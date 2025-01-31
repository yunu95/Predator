#include "Timer.h"


void TimerComponent::Update()
{
    if (m_isActivated)
    {
        m_elapsed += (isRealtime ? Time::GetDeltaTimeUnscaled() : Time::GetDeltaTime()) * m_localTimeScale;
        if (m_elapsed >= duration)
        {

            if (m_isRepeated == false)
            {
                m_elapsed = duration;
                onUpdate(m_elapsed / duration);
                onExpiration();
                m_isActivated = false;
                m_elapsed = 0.0f;
            }
            else
            {
                m_elapsed = 0.0f;
            }
            onCompleteFunction();
        }
        else
        {
            onUpdate(m_elapsed / duration);
        }
    }
}

//void TimerComponent::PlayFunction()
//{
//
//}
//
//void TimerComponent::StopFunction()
//{
//	yunutyEngine::Scene::getCurrentScene()->DestroyGameObject(GetGameObject());
//}

void TimerComponent::ActivateTimer()
{
    m_elapsed = 0.0f;
    m_isActivated = true;
    if (onActivation)
    {
        onActivation();
    }
    if (duration == 0)
    {
        onUpdate(1);
        onCompleteFunction();
        m_isActivated = false;
    }
}

void TimerComponent::StopTimer()
{
    m_isActivated = false;
    m_elapsed = 0.0f;
}
void TimerComponent::SetUsingRealtime(bool isRealtime)
{
    this->isRealtime = isRealtime;
}
