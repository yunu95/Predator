/// 2023. 10. 04 김상준
/// 편집 가능한 계층 구조를 확인하는 패널

#pragma once

#include "EditorPanel.h"

#include <memory>

namespace application
{
	namespace editor
	{
		class HierarchyPanel
			: public Panel
		{
		public:
			static HierarchyPanel& GetInstance();

			virtual ~HierarchyPanel();

			virtual void Initialize() override;
			virtual void Update(float ts) override;
			virtual void GUIProgress() override;
			virtual void Finalize() override;

		protected:
			static std::unique_ptr<HierarchyPanel> instance;

			HierarchyPanel();
			HierarchyPanel(const HierarchyPanel& copy) = delete;
			HierarchyPanel& operator=(const HierarchyPanel& copy) = delete;
		};
	}
}
