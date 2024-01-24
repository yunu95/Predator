/// 2024. 01. 04 김상준
/// Editor 에서 Scene 내용을 출력 할 카메라
/// 하나의 카메라만 사용할 것이기 때문에 Singleton 으로 구현

#pragma once

#include "Singleton.h"
#include "EditorInputManager.h"
#include "EditorEvents.h"

#include "Camera.h"

namespace application
{
	namespace editor
	{
		enum class CameraTypeState
		{
			None = 0,
			Editor,
			Game
		};

		enum class CameraPerspectiveState
		{
			None = 0,
			Free,
			Game
		};

		class EditorCamera
			: public Singleton<EditorCamera>
		{
			friend class Singleton<EditorCamera>;

		public:
			void Initialize(yunutyEngine::graphics::Camera* gameCam);
			void OnEvent(EditorEvents& event);
			void Update(float ts);

			// MainCamera 를 EditorCamera / GameCamera 로 각각 전환합니다.
			void SwitchCam();
			// GameCamera 시점에서 움직이도록 변경
			void SetGamePerspective();
			// 자유 모드로 전환
			void SetFreePerspective();
			// 카메라 움직임 속도 획득
			float GetCameraSpeed() const;

			CameraPerspectiveState GetGamePerspective() { return cameraPState; }

			yunuGI::Vector3 GetUpDirection() const;
			yunuGI::Vector3 GetRightDirection() const;
			yunuGI::Vector3 GetForwardDirection() const;

			const yunuGI::Vector3& GetPosition() const { return position; }

			yunuGI::Quaternion GetOrientation() const;

		private:
			EditorCamera() = default;


			void UpdateCameraView();

			CameraTypeState cameraTState = CameraTypeState::None;
			CameraPerspectiveState cameraPState = CameraPerspectiveState::None;
			yunutyEngine::graphics::Camera* editorCam = nullptr;
			yunutyEngine::graphics::Camera* gameCam = nullptr;
			EditorInputManager& eim = EditorInputManager::GetSingletonInstance();

			yunuGI::Matrix4x4 viewMatrix = yunuGI::Matrix4x4();

			yunuGI::Vector3 position = yunuGI::Vector3();
			yunuGI::Vector3 positionDelta = yunuGI::Vector3();

			yunuGI::Vector3 forwardDirection = yunuGI::Vector3();
			yunuGI::Vector3 rightDirection = yunuGI::Vector3();

			yunuGI::Vector3 focalPos = yunuGI::Vector3();
			float focalDistance = 0.0f;

			yunuGI::Vector2 mousePos = yunuGI::Vector2();
			yunuGI::Vector2 beforeMousePos = yunuGI::Vector2();

			float pitch = 0.0f;
			float pitchDelta = 0.0f;
			float yaw = 0.0f;
			float yawDelta = 0.0f;

			static float min_Speed;
			static float max_Speed;
			float defaultMoveSpeed = 0.03f;
			float rotationSpeed = 0.3f;
		};
	}
}
