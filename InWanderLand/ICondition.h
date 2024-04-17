/// 2024. 04. 05 김상준
/// Script 에서 실행 조건이 되는 Condition 의 Interface 입니다.

#pragma once

#include "Storable.h"
#include "Identifiable.h"

#include "IObserver.h"

#include "imgui.h"
#include "imgui_Utility.h"

namespace application
{
	enum class ConditionType
	{
		None,
		CinematicModeOn,
		CinematicModeOff,
	};

	struct ICondition
		: public Identifiable, public Storable, public IObserver
	{
		friend class Script;

		/// Observer 로 사용할 경우, 소멸자에서
		/// Target 의 Remove 를 호출해야 합니다.
		virtual ~ICondition() = default;

		/// dynamic_cast 가 아닌 switch case 로 동작하기 위한 함수입니다.
		virtual ConditionType GetType() const = 0;

		/// 특정 조건을 내부에서 설정하고, 해당 조건을 만족했는지에 대한 값을 반환합니다.
		virtual bool IsConditionMet() = 0;

		/// pointer 를 내부에서 할당하는 등 특정 데이터를
		/// 세팅하지 않을 경우에 문제가 된다면 해당 데이터를
		/// 파악하여 Valid 를 return 하는 함수를 override 해야합니다.
		virtual bool IsValid() { return true; }

		/// 특별히 관찰할 대상이 있고, 그 대상으로부터 처리할 이벤트가 있을 경우,
		/// 해당 함수를 작성해야 합니다.
		virtual void ProcessObervationEvent(ObservationTarget* target, ObservationEvent event) {}
	};
}

/// ImGui_DrawDataPopup 함수를 작성하여 Editor 에서 데이터 편집 기능을
/// 제공하여야 합니다.
#define DEFINE_CONDITION(Class) \
virtual ConditionType GetType() const override { return ConditionType::Class; } \
static void ImGui_DrawDataPopup(Condition_##Class* data);
