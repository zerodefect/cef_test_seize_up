
#include "browser.hpp"

// Libraries
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/dll/runtime_symbol_info.hpp>
#include <include/cef_render_handler.h>

#include "app.hpp"
#include "browser_client.hpp"
#include "render_handler.hpp"

///////////////////////////////////////////////////////////////////////////
browser::browser(const CefMainArgs& args) : m_args(args)
{}

///////////////////////////////////////////////////////////////////////////
bool browser::initialise(event *pEventDoWork)
{
    const boost::filesystem::path pathExecDir = boost::dll::program_location().parent_path();
    const boost::filesystem::path pathExecSubProcess = (pathExecDir / "cef_test_worker").string();

    if (!boost::filesystem::exists(pathExecSubProcess) || !boost::filesystem::is_regular_file(pathExecSubProcess))
    {
        std::cerr << "Cannot located renderer process at '" << pathExecSubProcess.string() << "'";
        return false;
    }

    // Specify the path for the sub-process executable.
    CefString(&m_settings.browser_subprocess_path).FromString(pathExecSubProcess.string());

    m_settings.no_sandbox = 1;
    m_settings.multi_threaded_message_loop = 0;
    m_settings.external_message_pump = 1;
    m_settings.windowless_rendering_enabled = 1;
    m_settings.uncaught_exception_stack_size = 10;

    if (!CefInitialize(m_args, m_settings, new app(pEventDoWork), nullptr))
    {
        std::cerr << "Failed to initialise the CEF renderer. Cannot continue";
        return false;
    }

    // Hard-coded values here.
    CefRefPtr<CefRenderHandler> apRenderHandler = new render_handler(1920,
                                                                     1080,
                                                                     [this](const void *pBuffer, const int width, const int height)
                                                                     {
                                                                         paint_callback(pBuffer, width, height);
                                                                     });
    CefRefPtr<CefClient> apClient = new browser_client(apRenderHandler);

    CefBrowserSettings browserSettings{};

    // Hard-coded values here.
    browserSettings.windowless_frame_rate = 50;

    CefWindowInfo window_info{};
    window_info.SetAsWindowless({});
    window_info.external_begin_frame_enabled = 1;

    CefRefPtr<CefBrowser> apBrowser =
        CefBrowserHost::CreateBrowserSync(window_info,
                                          apClient.get(),
                                          "https://darsa.in/fpsmeter/",
                                          browserSettings,
                                          nullptr, // CefDictionaryValue
                                          nullptr); // CefRequestContext

    m_apBrowser = std::move(apBrowser);
    m_apClient = std::move(apClient);

    m_bInit = true;
    return true;
}

///////////////////////////////////////////////////////////////////////////
bool browser::finalise()
{
    if (!m_bInit) { return false; }

    m_apBrowser = CefRefPtr<CefBrowser>();
    m_apClient = CefRefPtr<CefClient>();
    return true;
}

///////////////////////////////////////////////////////////////////////////
bool browser::schedule_external_frame()
{
    auto apBrowserHost = m_apBrowser->GetHost();
    assert(!!apBrowserHost);

    if (auto nFireCount = ++m_nFireCount; nFireCount % 250 == 0)
    {
        std::cout << "Scheduled frame - (250 times - ~5 secs)" << std::endl;
    }

    apBrowserHost->SendExternalBeginFrame();
    return true;
}

///////////////////////////////////////////////////////////////////////////
void browser::paint_callback(const void *pBuffer, const int width, const int height)
{
#if 1
    if (auto nPaintCount = ++m_nPaintCount; nPaintCount % 250 == 0)
    {
        std::cout << "Paint - (250 times)" << std::endl;
    }
#else
    std::cout << "Paint" << std::endl;
#endif
}
