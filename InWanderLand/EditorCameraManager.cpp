#include "EditorCameraManager.h"

#include "YunutyEngine.h"
#include "CameraEditorInstance.h"
#include "EditorCamera.h"

namespace application
{
	namespace editor
	{
		void CameraManager::RegisterCamera(CameraData* cam)
		{
			camList.insert(cam);
		}

		void CameraManager::EraseCamera(CameraData* cam)
		{
			camList.erase(cam);
		}

		CameraData* CameraManager::GetMainCam()
		{
			return mainCam;
		}

		void CameraManager::SetMainCam(CameraData* cam)
		{
			mainCam = cam;
		}

		void CameraManager::Clear()
		{
			if (EditorCamera::GetSingletonInstance().GetCameraTypeState() == CameraTypeState::Game)
			{
				EditorCamera::GetSingletonInstance().SwitchCam();
			}

			camList.clear();
			mainCam = nullptr;
		}
	}
}
