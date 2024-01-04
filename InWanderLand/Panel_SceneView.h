/// 2023. 10. 04 김상준
/// 에디터에서 편집을 위한 내용을 확인할 수 있는 View 패널

#pragma once

#include "Singleton.h"
#include "EditorPanel.h"

namespace application
{
	class Application;
}

namespace application
{
	namespace editor
	{
		class SceneViewPanel
			: public Panel, public Singleton<SceneViewPanel>
		{
		public:
			SceneViewPanel();
			virtual ~SceneViewPanel();

			virtual void Initialize() override;
			virtual void Update(float ts) override;
			virtual void GUIProgress() override;
			virtual void Finalize() override;

		private:
			Application* app;
			unsigned int rendererWidth;
			unsigned int rendererHeight;
		};
	}
}
