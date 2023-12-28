#include "StatusTimer.h"

void StatusTimer::Start()
{
	m_isActivated = false;
	m_isRepeated = false;

	m_elapsed = 0.0f;
	m_duration = 0.0f;
}

void StatusTimer::Update()
{
	if (m_isActivated)
	{
		m_elapsed += Time::GetDeltaTime();

		if (m_elapsed >= m_duration)
		{
			onCompleteFunction();

			if (!m_isRepeated)
			{
				onExpiration();
				m_isActivated = false;
				m_elapsed = 0.0f;
			}
			else
			{
				m_elapsed -= m_duration;
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