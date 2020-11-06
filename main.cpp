
#include <atomic>
#include <cassert>
#include <functional>
#include <iostream>
#include <memory>
#include <thread>

#include <include/cef_app.h>
#include <include/cef_render_handler.h>

#include <event2/event.h>
#include <event2/thread.h>

#include <X11/Xlib.h>

#include "browser.hpp"

///////////////////////////////////////////////////////////////////////////
void init_browser_event_callback(evutil_socket_t, short events, void *user_data);
void schedule_frame_event_callback(evutil_socket_t, short events, void *user_data);
void do_work_event_callback(evutil_socket_t, short events, void *user_data);

///////////////////////////////////////////////////////////////////////////
struct event_data
{
    browser *pBrowser{nullptr};
    event *pEventDoWork{nullptr};

    std::thread *pthread{nullptr};
    std::function<void()> *pfnThread{nullptr};
};

///////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
    CefMainArgs main_args(argc, argv);

    std::cout << "CEF Test v0.1" << std::endl;

    // Try to get a hold of the XDisplay to ensure that the frame buffer has started.
    const auto *const pchDisplayEnv = std::getenv("DISPLAY");
    {
        const auto bValidVar = pchDisplayEnv != nullptr;
        auto pcstream = bValidVar ? &std::cout : &std::cerr;
        (*pcstream) << "DISPLAY variable is '"
                    << (!bValidVar ? "<<NOT SET>>" : pchDisplayEnv)
                    << "'"
                    << std::endl;
    }

    if (pchDisplayEnv == nullptr)
    {
        std::cerr << "The DISPLAY variable is invalid. Cannot continue." << std::endl;
        return 1;
    }

    Display *pDisplay{};
    if (nullptr == (pDisplay=XOpenDisplay(pchDisplayEnv)))
    {
        std::cerr << "The X11 frame buffer is not available. Attempting to open display once per second for 10 seconds";
        for (std::size_t k = 0; k < 10; ++k)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            pDisplay = XOpenDisplay(pchDisplayEnv);
            if (pDisplay != nullptr)
            {
                std::cout << "Display available. Continuing." << std::endl;
                break;
            }
        }

        if (pDisplay == nullptr)
        {
            std::cerr << "Display not available after backing off. Cannot continue.";
            return 1;
        }
    }

    if (pDisplay != nullptr)
    {
        XCloseDisplay(pDisplay);
    }

    // According to a response on StackOverflow, this function must be called
    //      before any data is allocated by the libevent2 library:
    //      https://stackoverflow.com/questions/23657747/libevent-configuring-event-base-with-thread-support
    if (evthread_use_pthreads() < 0)
    {
        std::cerr << "Libevent API would not accept use of pthreads. Cannot continue";
        return 1;
    }

    std::unique_ptr<event_base, std::function<void(event_base *)>> apEventBase{event_base_new(),
                                                                               [](event_base *const pBase)
                                                                               {
                                                                                   if (pBase != nullptr)
                                                                                   {
                                                                                       event_base_free(pBase);
                                                                                   }
                                                                               }};

    if (!apEventBase)
    {
        std::cerr << "Couldn't get an libevent event_base. Cannot continue";
        return 1;
    }

    browser browser(main_args);

    std::atomic_bool abCancel{false};
    std::thread thread;
    std::function<void()> fnThread;
    event_data ed{.pBrowser = &browser, .pEventDoWork = nullptr, .pthread = &thread, .pfnThread = &fnThread};

    // 1. Event Do Work.
    std::unique_ptr<event, std::function<void(event *)>> apEHDoWork{event_new(apEventBase.get(), -1, EV_PERSIST | EV_READ, do_work_event_callback, (void *)&ed),
                                                                    [](event *const pEvent)
                                                                    {
                                                                        if (pEvent != nullptr)
                                                                        {
                                                                            event_free(pEvent);
                                                                        }
                                                                    }};
    if (apEHDoWork)
    {
        if (event_add(apEHDoWork.get(), nullptr) < 0)
        {
            std::cerr << "Failed to add signal event handler to event queue (DoWork()). Cannot continue.";
            return 1;
        }

        ed.pEventDoWork = apEHDoWork.get();
    }

    // 2. Event Sched Frame (explicit scheduling)
    std::unique_ptr<event, std::function<void(event *)>> apEHSchedFrame =
        std::unique_ptr<event, std::function<void(event *)>>{
            event_new(apEventBase.get(), -1, EV_PERSIST | EV_READ, schedule_frame_event_callback, (void *) &ed),
            [](event *const pEvent)
            {
                if (pEvent != nullptr)
                {
                    event_free(pEvent);
                }
            }};

    if (apEHSchedFrame)
    {
        if (event_add(apEHSchedFrame.get(), nullptr) < 0)
        {
            std::cerr << "Failed to add signal event handler to event queue (SchedFrame()). Cannot continue.";
            return 1;
        }

        fnThread = [pabCancel=&abCancel, pEHSchedFrame=apEHSchedFrame.get()]()
        {
            std::size_t n = 0;

            while (!(*pabCancel))
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(20));

                // Fire!
                event_active(pEHSchedFrame, EV_READ, 0);

                if (auto nFireCount = ++n; nFireCount % 250 == 0)
                {
                    std::cout << "Pump schedule frame event - (250 times - ~5 secs)" << std::endl;
                }
            }
        };
    }

    // 3. Event Initialise (explicit initialisation)
    std::unique_ptr<event, std::function<void(event *)>> apEHInit =
        std::unique_ptr<event, std::function<void(event *)>>{
            event_new(apEventBase.get(), -1, EV_PERSIST | EV_READ, init_browser_event_callback, (void *) &ed),
            [](event *const pEvent)
            {
                if (pEvent != nullptr)
                {
                    event_free(pEvent);
                }
            }};

    if (apEHInit)
    {
        if (event_add(apEHInit.get(), nullptr) < 0)
        {
            std::cerr << "Failed to add signal event handler to event queue (InitBrowser()). Cannot continue.";
            return 1;
        }

        event_active(apEHInit.get(), EV_READ, 0);
    }

    std::cout << "Running event loop." << std::endl;

    const auto nResult = event_base_dispatch(apEventBase.get());
    std::cout << "Finalising." << std::endl;

    abCancel=true;
    if (thread.joinable()) { thread.join(); }

    browser.finalise();

    if (nResult == 0u)
    {
        std::cout << "Application exited with value 0." << std::endl;
    }
    else
    {
        std::cerr << "Application exited with value "
                  << nResult
                  << "d"
                  << std::endl;
    }

    return static_cast<int>(nResult);

    return 0;
}

///////////////////////////////////////////////////////////////////////////
void init_browser_event_callback(evutil_socket_t, short events, void *user_data)
{
    auto *const pEv = (event_data*)user_data;
    if (pEv == nullptr) { assert(false); return; }

    auto *pBrowser = pEv->pBrowser;
    if (pBrowser == nullptr) { assert(false); return; }

    if (!pBrowser->initialise(pEv->pEventDoWork))
    {
        std::cerr << "Failed to initialise browser. Cannot continue." << std::endl;
        assert(false);
        exit(1);
    }

    // WARN: This is the important bit here. One must wait before
    //       calling SendExternalBeginFrame() after init.
    std::this_thread::sleep_for(std::chrono::milliseconds (1000));

    std::cout << "Starting thread as frame scheduler." << std::endl;
    (*pEv->pthread) = std::thread(*pEv->pfnThread);
}

///////////////////////////////////////////////////////////////////////////
void schedule_frame_event_callback(evutil_socket_t, short events, void *user_data)
{
    auto *const pEv = (event_data*)user_data;
    if (pEv == nullptr) { assert(false); return; }

    auto *pBrowser = pEv->pBrowser;
    if (pBrowser == nullptr) { assert(false); return; }

    // Explicit frame schedule.
    assert(CefCurrentlyOn(TID_UI));
    pBrowser->schedule_external_frame();
}

///////////////////////////////////////////////////////////////////////////
void do_work_event_callback(evutil_socket_t, short events, void *user_data)
{
    auto *const pEv = (event_data*)user_data;
    if (pEv == nullptr) { assert(false); return; }

    auto *pBrowser = pEv->pBrowser;
    if (pBrowser == nullptr) { assert(false); return; }

    //static std::atomic_size_t nFired = 0;
    //std::cout << "DoWork! " << (++nFired) << std::endl;

    assert(CefCurrentlyOn(TID_UI));
    CefRunMessageLoop();
}
