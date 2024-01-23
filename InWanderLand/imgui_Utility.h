/// 2024. 01. 18 김상준
/// ImGui 에서 사용할 수 있는 기능들을 모아서 정리한 Utility 파일

#pragma once

#include "imgui.h"

namespace application
{
	namespace editor
	{
		// Push / Pop 을 신경쓰지 않고 소멸자에서 Pop 을 수행하는 객체
		class SmartStyleVar
		{
		public:
			template<typename T>
			SmartStyleVar(ImGuiStyleVar styleVar, T value) { ImGui::PushStyleVar(styleVar, value); }
			~SmartStyleVar() { ImGui::PopStyleVar(); }

			SmartStyleVar(const SmartStyleVar&) = delete;
			SmartStyleVar& operator=(const SmartStyleVar&) = delete;
		};
	}
}