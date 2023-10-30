/// 2023. 10. 04 김상준
/// 에디터에서 선택된 카메라 시점의 View 를 확인할 수 있는 View 패널

#pragma once

#include "EditorPanel.h"

#include <memory>

namespace Application
{
	namespace Editor
	{
		class CameraViewPanel
			: public Panel
		{
		public:
			static CameraViewPanel& GetInstance();

			virtual ~CameraViewPanel();

			virtual void Initialize() override;
			virtual void Update(float ts) override;
			virtual void GUIProgress() override;
			virtual void Finalize() override;

		protected:
			static std::unique_ptr<CameraViewPanel> instance;

			CameraViewPanel();
			CameraViewPanel(const CameraViewPanel& copy) = delete;
			CameraViewPanel& operator=(const CameraViewPanel& copy) = delete;
		};
	}
}
