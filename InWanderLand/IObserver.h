/// 2024. 04. 17 김상준
/// 해당 객체를 참조하고 있는 대상들에게 특정 객체가 삭제되는 등의
/// 특별한 동작으로 인한 처리를 제공하는 Interface 클래스입니다.
/// 대상으로부터 동작을 관찰하는 관찰자 클래스입니다.

#pragma once

#include "ObservationEvent.h"

#include "ObservationTarget.h"

namespace application
{
	struct IObserver
	{
	public:
		virtual void ProcessObervationEvent(ObservationTarget* target, ObservationEvent event) = 0;
	};
}

