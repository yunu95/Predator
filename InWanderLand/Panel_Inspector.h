/// 2023. 10. 04 김상준
/// 선택한 특정 객체에 대하여 상세한 편집 내용을 확인할 수 있는 패널

#pragma once

#include "Singleton.h"
#include "EditorPanel.h"

#include <memory>

namespace application
{
	namespace editor
	{
		class InspectorPanel
			: public Panel, public Singleton<InspectorPanel>
		{
		public:
			InspectorPanel();
			virtual ~InspectorPanel();

			virtual void Initialize() override;
			virtual void Update(float ts) override;
			virtual void GUIProgress() override;
			virtual void Finalize() override;
		};
	}
}
