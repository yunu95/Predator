/// 2024. 03. 24 김상준
/// Main 카메라 처리를 위해 카메라들을 관리하는 Manager 클래스
/// 실제 Camera Component 로 동작하는 것이 아니라,
/// Editor 용 CameraData 로 동작합니다.

#pragma once
#include "Singleton.h"
#include "CameraData.h"

#include <unordered_set>

namespace application
{
	namespace editor
	{
		class CameraManager
			: public Singleton<CameraManager>
		{
			friend class Singleton<CameraManager>;

		public:
			void RegisterCamera(CameraData* cam);
			void EraseCamera(CameraData* cam);
			CameraData* GetMainCam();
			void SetMainCam(CameraData* cam);
			void Clear();

		private:
			CameraManager() = default;

			std::unordered_set<CameraData*> camList = std::unordered_set<CameraData*>();
			CameraData* mainCam = nullptr;
		};
	}
}
