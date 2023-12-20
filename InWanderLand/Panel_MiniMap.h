/// 2023. 10. 04 김상준
/// SceneView 패널이 전체 공간에서 현재 확인하고 있는 부분을 한 눈에 파악할 수 있는
/// MiniMap 을 제공해주는 패널

#pragma once

#include "EditorPanel.h"

#include <memory>

namespace application
{
	namespace editor
	{
		class MiniMapPanel
			: public Panel
		{
		public:
			static MiniMapPanel& GetInstance();

			virtual ~MiniMapPanel();

			virtual void Initialize() override;
			virtual void Update(float ts) override;
			virtual void GUIProgress() override;
			virtual void Finalize() override;

		protected:
			static std::unique_ptr<MiniMapPanel> instance;

			MiniMapPanel();
			MiniMapPanel(const MiniMapPanel& copy) = delete;
			MiniMapPanel& operator=(const MiniMapPanel& copy) = delete;
		};
	}
}
