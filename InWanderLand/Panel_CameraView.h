/// 2023. 10. 04 김상준
/// 에디터에서 선택된 카메라 시점의 View 를 확인할 수 있는 View 패널

#pragma once

#ifdef EDITOR
#include "Singleton.h"
#include "EditorPanel.h"

namespace application
{
	namespace editor
	{
		class CameraViewPanel
			: public Panel, public Singleton<CameraViewPanel>
		{
			friend class Singleton<CameraViewPanel>;

		public:
			virtual ~CameraViewPanel();

			virtual void Initialize() override;
			virtual void Update(float ts) override;
			virtual void GUIProgress() override;
			virtual void Finalize() override;

		private:
			CameraViewPanel();
		};
	}
}
#endif
