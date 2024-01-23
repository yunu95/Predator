/// 2023. 10. 11 김상준
/// application 에서 사용할 구성을 그룹화하여 통합 관리하기 위한
/// Layer 계층을 만드는 인터페이스

#pragma once

#include "EditorEvents.h"

namespace application
{
	class Layer
	{
	public:
		virtual void Initialize() = 0;
		virtual void Update(float ts) = 0;
		virtual void GUIProgress() = 0;
		virtual void Finalize() = 0;
		virtual void OnEvent(editor::EditorEvents& event) {};
	};
}
