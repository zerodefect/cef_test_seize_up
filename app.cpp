
#include "app.hpp"

#include "browser_process_handler.hpp"

//////////////////////////////////////////////////////////////////////////
app::app(event *pEventDoWork) : m_apBrowserProcessHandler(new browser_process_handler(pEventDoWork))
{}

//////////////////////////////////////////////////////////////////////////
app::~app() = default;

//////////////////////////////////////////////////////////////////////////
void app::OnBeforeCommandLineProcessing(const CefString &process_type, CefRefPtr<CefCommandLine> command_line)
{
    if (process_type.empty())
    {
        command_line->AppendSwitch("disable-gpu");
        command_line->AppendSwitch("disable-gpu-compositing");
        command_line->AppendSwitch("disable-gpu-vsync");
    }
}

//////////////////////////////////////////////////////////////////////////
CefRefPtr<CefBrowserProcessHandler> app::GetBrowserProcessHandler()
{
    return m_apBrowserProcessHandler;
}