
#ifndef BROWSER_PROCESS_HANDLER_HPP
#define BROWSER_PROCESS_HANDLER_HPP

// Libraries
#include <include/cef_browser_process_handler.h>

#include <event2/event.h>

///////////////////////////////////////////////////////////////////////////
class browser_process_handler final : public CefBrowserProcessHandler
{
public:

    browser_process_handler() = delete;

    browser_process_handler(event *pEventDoWork);

    void OnScheduleMessagePumpWork(int64 delay_ms) override;

private:

    IMPLEMENT_REFCOUNTING(browser_process_handler);

    event *const m_pEventDoWork{nullptr};

    std::size_t m_nFireCount{0};
};

#endif //BROWSER_PROCESS_HANDLER_HPP
