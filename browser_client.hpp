
#ifndef BROWSER_CLIENT_HPP
#define BROWSER_CLIENT_HPP

// Libraries
#include <utility>

#include <include/cef_client.h>

// Forward Declarations

//////////////////////////////////////////////////////////////////////////
class browser_client final : public virtual CefClient
{
public:

    browser_client(const CefRefPtr<CefRenderHandler> &renderHandler);

    CefRefPtr<CefRenderHandler> GetRenderHandler() override;

IMPLEMENT_REFCOUNTING(browser_client);

private:

    const CefRefPtr<CefRenderHandler> m_renderHandler;
};

#endif //BROWSER_CLIENT_HPP
