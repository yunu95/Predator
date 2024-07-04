#include "ObservationTarget.h"

#include "IObserver.h"

namespace application
{
	ObservationTarget::~ObservationTarget()
	{
		DispatchObervationEvent(ObservationEvent::Destroy);
	}

	bool ObservationTarget::RegisterObserver(IObserver* observer)
	{
		if (observer == nullptr)
		{
			return false;
		}

		observerList.insert(observer);
		return true;
	}

	bool ObservationTarget::RemoveObserver(IObserver* observer)
	{
		if (!observerList.contains(observer))
		{
			return false;
		}
		observerList.erase(observer);
		return true;
	}

	void ObservationTarget::DispatchObervationEvent(ObservationEvent event)
	{
		for (auto& each : observerList)
		{
			each->ProcessObervationEvent(this, event);
		}
	}
}