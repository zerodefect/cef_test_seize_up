#ifndef BROWSER_HPP
#define BROWSER_HPP

#include <include/cef_client.h>
#include <include/internal/cef_linux.h>
#include <include/internal/cef_ptr.h>

class CefBrowser;
class CefClient;

struct event;

///////////////////////////////////////////////////////////////////////////
class browser final
{
public:

    browser() = delete;

    browser(const CefMainArgs& args);

    bool initialise(event *pEventDoWork);

    bool finalise();

    bool schedule_external_frame();

private:


    void paint_callback(const void *pBuffer, int width, int height);

    const CefMainArgs& m_args;

    CefSettings m_settings{};

    CefRefPtr<CefBrowser> m_apBrowser;
    CefRefPtr<CefClient> m_apClient;

    std::size_t m_nFireCount{0};
    std::size_t m_nPaintCount{0};

    bool m_bInit{false};
};


#endif //BROWSER_HPP
