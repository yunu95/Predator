#include "Timer.h"


void Timer::Update()
{
    if (m_isActivated)
    {
        m_elapsed += isRealtime ? Time::GetDeltaTimeUnscaled() : Time::GetDeltaTime();
        if (m_elapsed >= m_duration)
        {
            onCompleteFunction();

            if (m_isRepeated == false)
            {
                m_elapsed = m_duration;
                onUpdate(m_elapsed / m_duration);
                onExpiration();
                m_isActivated = false;
                m_elapsed = 0.0f;
            }
            else
            {
                m_elapsed = 0.0f;
            }
        }
        else
        {
            onUpdate(m_elapsed / m_duration);
        }
    }
}

void Timer::ActivateTimer()
{
    m_elapsed = 0.0f;
    m_isActivated = true;
    onActivation();
}

void Timer::StopTimer()
{
    m_isActivated = false;
    m_elapsed = 0.0f;
}
void Timer::SetUsingRealtime(bool isRealtime)
{
    this->isRealtime = isRealtime;
}
