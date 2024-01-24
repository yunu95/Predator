#include "InWanderLand.h"
#include "StatusTimer.h"

void StatusTimer::Start()
{

}

void StatusTimer::Update()
{
	if (m_isActivated)
	{
		onUpdate();

		m_elapsed += Time::GetDeltaTime();

		if (m_elapsed >= m_duration)
		{
			onCompleteFunction();

			if (m_isRepeated == false)
			{
				onExpiration();
				m_isActivated = false;
				m_elapsed = 0.0f;
			}
			else
			{
				m_elapsed = 0.0f;
			}
		}
	}
}

void StatusTimer::ActivateTimer()
{
	m_elapsed = 0.0f;
	m_isActivated = true;
}

void StatusTimer::StopTimer()
{
	m_isActivated = false;
	m_elapsed = 0.0f;
}
