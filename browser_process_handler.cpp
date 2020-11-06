
#include "browser_process_handler.hpp"

// Libraries
#include <atomic>
#include <cassert>
#include <iostream>

//////////////////////////////////////////////////////////////////////////
browser_process_handler::browser_process_handler(event *pEventDoWork) : m_pEventDoWork(pEventDoWork)
{}

//////////////////////////////////////////////////////////////////////////
void browser_process_handler::OnScheduleMessagePumpWork(int64 delay_ms)
{
    assert(m_pEventDoWork != nullptr);
    if (m_pEventDoWork == nullptr)
    {
        assert(false);
        CefBrowserProcessHandler::OnScheduleMessagePumpWork(delay_ms);
        return;
    }

    if (auto nFireCount = ++m_nFireCount; nFireCount % 250 == 0)
    {
        std::cout << "Pump do work event - (250 times)" << std::endl;
    }

    //static std::atomic_size_t nFired = 0;
    //std::cout << "FireDoWork! " << (++nFired) << std::endl;

    // Not seen this be NON-ZERO. If it is, reconsider.
    assert(delay_ms == 0);
    //assert(0==event_pending(m_pEventDoWork, EV_READ, nullptr));
    event_active(m_pEventDoWork, EV_READ, 0);
}