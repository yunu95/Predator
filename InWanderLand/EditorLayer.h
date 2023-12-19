/// 2023. 10. 05 김상준
/// 에디터에서 사용할 모든 내용을 모아둔 클래스
/// 해당 레이어를 Release 모드에서 사용하지 않는 것으로
/// Release 모드에서는 에디터에 대한 진입이 불가능 하도록 함

#pragma once

#include <vector>
#include <memory>
#include <functional>

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
			static void AssignTestInitializer(std::function<void()> testInitializer);

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
				PREVIEW,
				MINIMAP,
				SCENEVIEW,
				CAMERAVIEW,
				PALETTE,

				/// Size를 자동으로 넣기 위해 사용하는 enum
				/// 첫 패널이 0 이고, 모든 간격이 1일 때에 가능함
				Size
			};

			static std::function<void()> testInitializer;

			void UI_DrawMenubar();

			CommandManager& cm = CommandManager::GetInstance();
			std::vector<Panel*> editorPanelList;
		};
	}
}
