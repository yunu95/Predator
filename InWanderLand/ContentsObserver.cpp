#include "ContentsObserver.h"
#include "ContentsObservee.h"

void ContentsObserver::RegisterObservee(ContentsObservee* p_observee)
{
	if (p_observee->isSingletonComponent)
		m_initializingContainer.push_back(p_observee);
	else
		m_destroyingContainer.push_back(p_observee);

}

void ContentsObserver::PlayObservee() const
{
	for (auto itr = m_destroyingContainer.begin(); itr != m_destroyingContainer.end(); itr++)
	{
		(*itr)->PlayFunction();
	}

	for (auto itr = m_initializingContainer.begin(); itr != m_initializingContainer.end(); itr++)
	{
		(*itr)->PlayFunction();
	}
}

void ContentsObserver::StopObservee() const
{
	for (auto each : m_initializingContainer)
	{
		each->StopFunction();
	}

	for (auto each : m_destroyingContainer)
	{
		each->StopFunction();
	}
}

void ContentsObserver::ClearObservees()
{
	for (auto itr = m_destroyingContainer.begin(); itr != m_destroyingContainer.end();)
	{
		itr = m_destroyingContainer.erase(itr);
	}

	for (auto each : m_initializingContainer)
	{
		each->isOncePaused = true;
	}
}
