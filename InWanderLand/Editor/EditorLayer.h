/// 2023. 10. 05 김상준
/// 에디터에서 사용할 모든 내용을 모아둔 클래스
/// 해당 레이어를 Release 모드에서 사용하지 않는 것으로
/// Release 모드에서는 에디터에 대한 진입이 불가능 하도록 함

#pragma once

#include <vector>

#include "Panel/EditorPanel.h"

namespace Editor
{
	class EditorLayer
	{
	public:
		void Initialize();
		void Update(float ts);
		void Render();
		void Finalize();
		//void OnEvent(Events& e);

	private:
		std::vector<Panel*> editorPanelList;
	};
}
