/// 2023. 10. 05 김상준
/// 에디터에서 사용할 모든 내용을 모아둔 클래스
/// 해당 레이어를 Release 모드에서 사용하지 않는 것으로
/// Release 모드에서는 에디터에 대한 진입이 불가능 하도록 함

#pragma once

#include <vector>
#include <memory>

#include "Layer.h"
#include "CommandManager.h"
#include "EditorPanel.h"

namespace Application
{
	namespace Editor
	{
		class EditorLayer
			: public Layer
		{
		public:
			virtual void Initialize() override;
			// virtual void EventProgress(Events& e) override;
			virtual void Update(float ts) override;
			virtual void GUIProgress() override;
			virtual void Finalize() override;

		private:
			enum class Panel_List
			{
				HIERARCHY	= 0,
				INSPECTOR,
				MINIMAP,
				SCENEVIEW,
				CAMERAVIEW
			};

			void UI_DrawMenubar();

			CommandManager& cm = CommandManager::GetInstance();
			std::vector<Panel*> editorPanelList;
		};
	}
}
