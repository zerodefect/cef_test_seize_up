
#ifndef APP_WORKER_HPP
#define APP_WORKER_HPP

#include <include/cef_app.h>

//////////////////////////////////////////////////////////////////////////
class app_worker final : public virtual CefApp
{
public:
    app_worker();

    ~app_worker() override;

private:

    IMPLEMENT_REFCOUNTING(app_worker);
};

#endif //APP_WORKER_HPP
