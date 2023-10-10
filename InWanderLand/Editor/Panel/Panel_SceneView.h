/// 2023. 10. 04 김상준
/// 에디터에서 편집을 위한 내용을 확인할 수 있는 View 패널

#pragma once

#include "EditorPanel.h"

namespace Editor
{
	class SceneViewPanel
		: public Panel
	{
	public:
		virtual ~SceneViewPanel();

		virtual void Initialize() override;
		virtual void Update(float ts) override;
		virtual void DrawPanel() override;
		virtual void Finalize() override;
		
		static SceneViewPanel& GetInstance();

	protected:
		SceneViewPanel();
		SceneViewPanel(const SceneViewPanel& copy) = delete;
		SceneViewPanel& operator=(const SceneViewPanel& copy) = delete;

		static SceneViewPanel* instance;
	};
}
