/// 2024. 04. 17 김상준
/// 해당 객체를 참조하고 있는 대상들에게 특정 객체가 삭제되는 등의
/// 특별한 동작으로 인한 처리를 제공하는 클래스입니다.
/// 관찰할 대상인 클래스입니다.

#pragma once

#include "ObservationEvent.h"

#include <unordered_set>

namespace application
{
	struct IObserver;
}

namespace application
{
	class ObservationTarget
	{
	public:
		virtual ~ObservationTarget();
		bool RegisterObserver(IObserver* observer);
		bool RemoveObserver(IObserver* observer);
		void DispatchObervationEvent(ObservationEvent event);
	private:
		std::unordered_set<IObserver*> observerList = std::unordered_set<IObserver*>();
	};
}

