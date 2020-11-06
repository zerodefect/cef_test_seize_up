
#ifndef APP_HPP
#define APP_HPP

#include <include/cef_app.h>

#include <event2/event.h>

//////////////////////////////////////////////////////////////////////////
class app final : public virtual CefApp
{
public:
    app() = delete;

    app(event *pEventDoWork);

    ~app() override;

    void OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr<CefCommandLine> command_line) override;

    CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override;

private:

    IMPLEMENT_REFCOUNTING(app);

    CefRefPtr<CefBrowserProcessHandler> m_apBrowserProcessHandler;
};

#endif //APP_HPP
