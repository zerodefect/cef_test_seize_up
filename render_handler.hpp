
#ifndef RENDER_HANDLER_HPP
#define RENDER_HANDLER_HPP

// Libraries
#include <cstdint>
#include <functional>
#include <utility>

#include <include/cef_app.h>
#include <include/cef_client.h>

// Forward Declarations


///////////////////////////////////////////////////////////////////////////
class render_handler final : public virtual CefRenderHandler
{
public:

    typedef std::function<void(const void *, int, int)> render_callback;

    render_handler() = delete;

    render_handler(const uint32_t nWidth,
                       const uint32_t nHeight,
                       render_callback &&callback) :
        m_nWidth(nWidth),
        m_nHeight(nHeight),
        m_fnRenderCallback(std::forward<render_callback>(callback))
    {

    }

    ~render_handler() override = default;

    void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override
    {
        rect.Set(0, // x
                 0, // y
                 static_cast<int>(m_nWidth), // width
                 static_cast<int>(m_nHeight)); // height
    }

    void OnPaint(CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType type,
                 const CefRenderHandler::RectList &dirtyRects, const void *buffer, int width, int height) override
    {
        if (m_fnRenderCallback)
        {
            m_fnRenderCallback(buffer, width, height);
        }
    }

private:
    // Include the default reference counting implementation.
IMPLEMENT_REFCOUNTING(render_handler);

    const uint32_t m_nWidth{0};
    const uint32_t m_nHeight{0};
    const render_callback m_fnRenderCallback;
};

#endif //RENDER_HANDLER_HPP
