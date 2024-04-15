/// 2024. 04. 05 김상준
/// Script 에서 실행할 단위가 되는 Action 의 Interface 입니다.
/// IAction 은 CoroutineObject 를 반환하는 Coroutine 함수
/// DoAction 을 작성해야만 합니다.

#pragma once

#include "Storable.h"
#include "Identifiable.h"
#include "CoroutineObject.h"

#include "imgui.h"
#include "imgui_Utility.h"

namespace application
{
	enum class ActionType
	{
		None,
		Example,
		WaitForSeconds,
		WaitForRealSeconds,
	};

	struct IAction
		: public Identifiable, public Storable
	{
		virtual ~IAction() = default;

		/// dynamic_cast 가 아닌 switch case 로 동작하기 위한 함수입니다.
		virtual ActionType GetType() const = 0;

		/// Coroutine 으로 실행할 함수입니다.
		virtual CoroutineObject<void> DoAction() = 0;
	};
}

/// ImGui_DrawDataPopup 함수를 작성하여 Editor 에서 데이터 편집 기능을
/// 제공하여야 합니다.
#define DEFINE_ACTION(Class) \
virtual ActionType GetType() const override { return ActionType::Class; } \
static void ImGui_DrawDataPopup(Action_##Class* data);