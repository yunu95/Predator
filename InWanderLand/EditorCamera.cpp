#include "InWanderLand.h"
#include "EditorCamera.h"

#include "YunutyEngine.h"

// 게임 엔진에서 사용할 Component 용 EditorCamera
class EditorCameraComponent
	: public yunutyEngine::graphics::Camera
{
	friend class application::editor::EditorCamera;
private:
	Vector3d euler;
};


namespace application
{
	namespace editor
	{
		void EditorCamera::Initialize(yunutyEngine::graphics::Camera* gameCam)
		{
			// Game 카메라 세팅
			this->gameCam = gameCam;
			auto gcts = gameCam->GetTransform();

			// Editor 카메라를 Scene에 생성하고 해당 카메라를 메인으로 변경
			auto scene = yunutyEngine::Scene::getCurrentScene();
			auto ecc = scene->AddGameObject()->AddComponent<EditorCameraComponent>();
			auto ts = ecc->GetTransform();
			ts->SetWorldPosition(gcts->GetWorldPosition());
			ts->SetWorldRotation(gcts->GetWorldRotation());
			ecc->SetCameraMain();

			editorCam = ecc;
			cameraTState = CameraTypeState::Editor;
			cameraPState = CameraPerspectiveState::Game;
		}

		void EditorCamera::SwitchCam()
		{
			switch (cameraTState)
			{
				case application::editor::EditorCamera::CameraTypeState::Editor:
				{
					cameraTState = CameraTypeState::Game;
					gameCam->SetCameraMain();
					break;
				}
				case application::editor::EditorCamera::CameraTypeState::Game:
				{
					cameraTState = CameraTypeState::Editor;
					editorCam->SetCameraMain();
					break;
				}
				default:
					break;
			}
		}

		void EditorCamera::Update(float ts)
		{
			if (eim.IsKeyboardUp(KeyCode::S))
			{
				SwitchCam();
			}
		}

		void EditorCamera::SetGamePerspective()
		{
			if (cameraPState == CameraPerspectiveState::Game)
			{
				return;
			}
			else
			{
				auto ts = editorCam->GetTransform();
				auto gcts = gameCam->GetTransform();
				auto pos = ts->GetWorldPosition();
				pos.y = gcts->GetWorldPosition().y;
				ts->SetWorldPosition(pos);
				ts->SetWorldRotation(gcts->GetWorldRotation());

				cameraPState = CameraPerspectiveState::Game;
				return;
			}
		}

		void EditorCamera::SetFreePerspective()
		{
			if (cameraPState == CameraPerspectiveState::Free)
			{
				return;
			}
			else
			{
				cameraPState = CameraPerspectiveState::Free;
				return;
			}
		}
	}
}
