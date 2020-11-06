
#include "browser_client.hpp"



//////////////////////////////////////////////////////////////////////////
browser_client::browser_client(const CefRefPtr<CefRenderHandler> &renderHandler) : m_renderHandler(renderHandler)
{}

//////////////////////////////////////////////////////////////////////////
CefRefPtr<CefRenderHandler> browser_client::GetRenderHandler()
{
    return m_renderHandler;
}
