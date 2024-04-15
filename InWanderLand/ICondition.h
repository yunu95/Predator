/// 2024. 04. 05 김상준
/// Script 에서 실행 조건이 되는 Condition 의 Interface 입니다.

#pragma once

#include "Storable.h"
#include "Identifiable.h"

#include "imgui.h"
#include "imgui_Utility.h"

namespace application
{
	enum class ConditionType
	{
		None,
	};

	struct ICondition
		: public Identifiable, public Storable
	{
		friend class Script;

		virtual ~ICondition() = default;

		/// dynamic_cast 가 아닌 switch case 로 동작하기 위한 함수입니다.
		virtual ConditionType GetType() const = 0;

		/// 특정 조건을 내부에서 설정하고, 해당 조건을 만족했는지에 대한 값을 반환합니다.
		virtual bool IsConditionMet() = 0;
	};
}

/// ImGui_DrawDataPopup 함수를 작성하여 Editor 에서 데이터 편집 기능을
/// 제공하여야 합니다.
#define DEFINE_CONDITION(Class) \
virtual ConditionType GetType() const override { return ConditionType::Class; } \
static void ImGui_DrawDataPopup(Condition_##Class* data);
