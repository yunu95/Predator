#include "ContentsObserver.h"
#include "ContentsObservee.h"

void ContentsObserver::RegisterObservee(ContentsObservee* p_observee)
{
	m_observeeContainer.push_back(p_observee);
}

void ContentsObserver::PlayObservee() const
{
	for (auto itr = m_observeeContainer.begin(); itr != m_observeeContainer.end(); itr++)
	{
		(*itr)->PlayFunction();
	}
}

void ContentsObserver::StopObservee() const
{
	for (auto each : m_observeeContainer)
	{
		each->StopFunction();
	}
}

void ContentsObserver::ClearObservees()
{
	for (auto itr = m_observeeContainer.begin(); itr != m_observeeContainer.end();)
	{
		if ((*itr)->isSingletonComponent)
		{
			(*itr)->isOncePaused = true;
			itr++;
			continue;
		}
		else
			itr = m_observeeContainer.erase(itr);
	}
}
