/// 2024. 01. 04 김상준
/// Editor 에서 Scene 내용을 출력 할 카메라
/// 하나의 카메라만 사용할 것이기 때문에 Singleton 으로 구현

#pragma once

#include "Singleton.h"
#include "EditorInputManager.h"
#include "EditorEvents.h"
#include "EditorMath.h"

#include "Camera.h"



namespace yunutyEngine
{
	class GameObject;
}

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
			void Initialize();
			void OnEvent(EditorEvents& event);
			void Update(float ts);

			// Input에 대한 처리용 플래그를 설정합니다.
			void SetInputUpdate(bool inputUpdate) { this->inputUpdate = inputUpdate; }
			// MainCamera 를 EditorCamera / GameCamera 로 각각 전환합니다.
			void SwitchCam();
			// GameCamera 시점에서 움직이도록 변경
			void SetGamePerspective();
			// 자유 모드로 전환
			void SetFreePerspective();
			// 카메라 움직임 속도 획득
			float GetCameraSpeed() const;

			CameraTypeState GetCameraTypeState() { return cameraTState; }
			CameraPerspectiveState GetGamePerspective() { return cameraPState; }

			yunutyEngine::graphics::Camera* GetGameCam() const { return gameCam; }
			void SetGameCam(yunutyEngine::graphics::Camera* cam) { gameCam = cam; }

			void OnPlayContents();
			void OnPauseContents();
			void OnResumeContents();
			void OnStopContents();

			void ReloadGameCamera();

			yunuGI::Vector3 GetUpDirection() const;
			yunuGI::Vector3 GetRightDirection() const;
			yunuGI::Vector3 GetForwardDirection() const;
			yunuGI::Vector3 GetPosition() const;
			yunuGI::Quaternion GetOrientation() const;
			yunuGI::Vector3 GetScale() const;

			yunuGI::Matrix4x4 GetWTM() const;
			yunuGI::Matrix4x4 GetVTM() const;
			yunuGI::Matrix4x4 GetPTM() const;

			yunuGI::ICamera& GetGI();

			void ChangeVTM(const yunuGI::Matrix4x4& vtm);

		private:
			EditorCamera() = default;

			void UpdateCameraView();
			void UpdateGizmo();

			GameObject* axisGizmo = nullptr;

			CameraTypeState cameraTState = CameraTypeState::None;
			CameraPerspectiveState cameraPState = CameraPerspectiveState::None;
			yunutyEngine::graphics::Camera* editorCam = nullptr;
			yunutyEngine::graphics::Camera* gameCam = nullptr;
			yunutyEngine::graphics::Camera* playCam = nullptr;
			EditorInputManager& eim = EditorInputManager::GetSingletonInstance();

			yunuGI::Vector3 position = yunuGI::Vector3();
			yunuGI::Vector3 positionDelta = yunuGI::Vector3();

			yunuGI::Vector3 forwardDirection = yunuGI::Vector3();
			yunuGI::Vector3 rightDirection = yunuGI::Vector3();

			yunuGI::Vector3 focalPos = yunuGI::Vector3();
			float focalDistance = 0.0f;

			yunuGI::Vector2 mousePos = yunuGI::Vector2();
			yunuGI::Vector2 beforeMousePos = yunuGI::Vector2();

			bool inputUpdate = false;

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
