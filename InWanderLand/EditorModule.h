/// 2023. 12. 27 김상준
/// Editor 에서 사용하는 Module 들의 공통된 기능 제공을 위한 추상클래스
/// Module 은 Panel 을 상속받음

#pragma once

#include "EditorPanel.h"

#pragma region Macro Function
// activation 플래그를 통해서 활성화를 체크하여 리턴하도록 하는 매크로 함수 지원
#define CheckActivation() \
if (!activation) \
{ \
	return; \
}
#pragma endregion

namespace application
{
	namespace editor
	{
		class EditorModule
			: public Panel
		{
		public:
			virtual ~EditorModule() = default;

			inline bool GetActivation() const { return activation; }
			inline void SetActivation(bool activation) { this->activation = activation; }

		protected:
			bool activation = false;
		};
	}
}
