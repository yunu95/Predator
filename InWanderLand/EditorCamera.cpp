#include "InWanderLand.h"
#include "EditorCamera.h"
#include "EditorMath.h"
#include "MouseEvents.h"

#include "YunutyEngine.h"
#include "DebugMeshes.h"

#include "InstanceManager.h"
#include "CameraData.h"
#include "EditorCameraManager.h"
#include "Application.h"
#include "PaletteManager.h"
#include "Palette.h"
#include "Panel_Palette.h"
#include "RTSCam.h"

#include <DirectXMath.h>

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
		float EditorCamera::min_Speed = 0.002f;
		float EditorCamera::max_Speed = 2.0f;

		void EditorCamera::Initialize()
		{
			// Editor 카메라를 Scene에 생성하고 해당 카메라를 메인으로 변경
			auto scene = yunutyEngine::Scene::getCurrentScene();
			auto ecc = scene->AddGameObject()->AddComponent<EditorCameraComponent>();
			ecc->SetCameraMain();
			auto ts = ecc->GetTransform();

			auto camData = CameraManager::GetSingletonInstance().GetMainCam();

			if (camData)
			{
				gameCam = camData->GetCameraComponent();
				auto gcts = gameCam->GetTransform();
				auto xzForward = gcts->GetWorldRotation().Forward().Normalized();
				xzForward.y = 0;
				ts->SetWorldPosition(gcts->GetWorldPosition() + xzForward.Normalized() * 2);
				ts->SetWorldRotation(gcts->GetWorldRotation());
				ts->SetWorldScale(gcts->GetWorldScale());
				cameraPState = CameraPerspectiveState::Free;
			}
			else
			{
				Vector3d pos = Vector3d( 0, 10, -5 );
				Quaternion rot = Quaternion(Vector3d( 60, 0, 0 ));
				Vector3d scl = Vector3d( 1, 1, 1 );

				ts->SetWorldPosition(pos);
				ts->SetWorldRotation(rot);
				ts->SetWorldScale(scl);

				auto cam = InstanceManager::GetSingletonInstance().CreateInstance<CameraData>("DefaultCamera");
				cam->pod.position.x = pos.x;
				cam->pod.position.y = pos.y + 1;
				cam->pod.position.z = pos.z;
				cam->pod.rotation.w = rot.w;
				cam->pod.rotation.x = rot.x;
				cam->pod.rotation.y = rot.y;
				cam->pod.rotation.z = rot.z;
				cam->pod.scale.x = scl.x;
				cam->pod.scale.y = scl.y;
				cam->pod.scale.z = scl.z;
				cam->pod.isMain = true;

				cam->ApplyAsPaletteInstance();
				gameCam = cam->GetCameraComponent();

				CameraManager::GetSingletonInstance().SetMainCam(cam);

				cameraPState = CameraPerspectiveState::Free;
			}

			editorCam = ecc;
			cameraTState = CameraTypeState::Editor;

			auto ecPos = ts->GetWorldPosition();
			position.x = ecPos.x;
			position.y = ecPos.y;
			position.z = ecPos.z;

			auto rot = ts->GetWorldRotation();
			glm::quat glmRot = glm::quat(rot.w, -rot.x, -rot.y, rot.z);

			pitch = -glm::pitch(glmRot);
			yaw = -glm::yaw(glmRot);

			forwardDirection = GetForwardDirection();
			rightDirection = GetRightDirection();

			ecFov = editorCam->GetGI().GetVerticalFOV();
			ecNear = editorCam->GetGI().GetNear();
			ecFar = editorCam->GetGI().GetFar();
			editorCam->GetGI().GetResolution(&ecWidth, &ecHeight);

			gcFov = gameCam->GetGI().GetVerticalFOV();
			gcNear = gameCam->GetGI().GetNear();
			gcFar = gameCam->GetGI().GetFar();
			gameCam->GetGI().GetResolution(&gcWidth, &gcHeight);

			if (cameraPState == CameraPerspectiveState::Game)
			{
				cameraPState = CameraPerspectiveState::Free;
				SetGamePerspective();
			}
		}

		void EditorCamera::OnEvent(EditorEvents& event)
		{
			if (event.GetEventType() == EventType::MouseMove)
			{
				mousePos.x = static_cast<MouseMoveEvent*>(&event)->GetPosX();
				mousePos.y = static_cast<MouseMoveEvent*>(&event)->GetPosY();
			}
		}

		void EditorCamera::SwitchCam()
		{
			switch (cameraTState)
			{
				case application::editor::CameraTypeState::Editor:
				{
					if (gameCam)
					{
						if (Application::GetInstance().IsContentsPlaying())
						{
							return;
						}

						cameraTState = CameraTypeState::Game;
						for (auto each : InstanceManager::GetSingletonInstance().GetList<CameraData>())
						{
							each->ApplyAsPaletteInstance()->HideEditorInstance();
						}
						gameCam->SetCameraMain();
						palette::CameraPalette::SingleInstance().Reset();
					}
					break;
				}
				case application::editor::CameraTypeState::Game:
				{
					cameraTState = CameraTypeState::Editor;
					for (auto each : InstanceManager::GetSingletonInstance().GetList<CameraData>())
					{
						each->ApplyAsPaletteInstance()->ShowEditorInstance();
					}
					editorCam->SetCameraMain();
					break;
				}
				default:
					break;
			}
		}

		void EditorCamera::Update(float ts)
		{
			yunuGI::Vector2 deltaPos;
			deltaPos.x = (mousePos.x - beforeMousePos.x) * 0.002f;
			deltaPos.y = (mousePos.y - beforeMousePos.y) * 0.002f;

			static Vector3d targetPos = Vector3d();

			if (inputUpdate)
			{
				if (eim.IsKeyboardUp(KeyCode::C))
				{
					SwitchCam();
				}

				if (cameraTState == application::editor::CameraTypeState::Editor
					&& cameraPState == application::editor::CameraPerspectiveState::Free
					&& eim.IsKeyboardUp(KeyCode::F))
				{
					targetPos = Vector3d();
					auto& selections = palette::PaletteManager::GetSingletonInstance().GetCurrentPalette()->GetSelections();
					for(auto& each : selections)
					{
						if (each->GetPaletteInstance())
						{
							targetPos += each->GetPaletteInstance()->GetTransform()->GetWorldPosition();
							moveFocus = true;
						}
					}
					
					if (moveFocus)
					{
						targetPos /= selections.size();
						targetPos -= editorCam->GetTransform()->GetWorldRotation().Forward().Normalized() * focalDistance;
					}
				}

				if (eim.IsMouseButtonDown(MouseCode::Right))
				{
					switch (cameraTState)
					{
						case application::editor::CameraTypeState::Editor:
						{
							const float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
							const float speed = GetCameraSpeed();

							switch (cameraPState)
							{
								case application::editor::CameraPerspectiveState::Free:
								{
									if (eim.IsKeyboardDown(KeyCode::Q))
									{
										positionDelta.y -= speed * yunuGI::Vector3(0.f, yawSign, 0.f).y;
									}
									if (eim.IsKeyboardDown(KeyCode::E))
									{
										positionDelta.y += speed * yunuGI::Vector3(0.f, yawSign, 0.f).y;
									}
									if (eim.IsKeyboardDown(KeyCode::S))
									{
										positionDelta.x -= speed * forwardDirection.x;
										positionDelta.y -= speed * forwardDirection.y;
										positionDelta.z -= speed * forwardDirection.z;
									}
									if (eim.IsKeyboardDown(KeyCode::W))
									{
										positionDelta.x += speed * forwardDirection.x;
										positionDelta.y += speed * forwardDirection.y;
										positionDelta.z += speed * forwardDirection.z;
									}
									if (eim.IsKeyboardDown(KeyCode::A))
									{
										positionDelta.x -= speed * rightDirection.x;
										positionDelta.y -= speed * rightDirection.y;
										positionDelta.z -= speed * rightDirection.z;
									}
									if (eim.IsKeyboardDown(KeyCode::D))
									{
										positionDelta.x += speed * rightDirection.x;
										positionDelta.y += speed * rightDirection.y;
										positionDelta.z += speed * rightDirection.z;
									}

									float maxRate = 0.12f;
									yawDelta += glm::clamp(yawSign * deltaPos.x * rotationSpeed, -maxRate, maxRate);
									pitchDelta += glm::clamp(deltaPos.y * rotationSpeed, -maxRate, maxRate);
									break;
								}
								case application::editor::CameraPerspectiveState::Game:
								{
									if (eim.IsKeyboardDown(KeyCode::Q))
									{
										positionDelta.y -= speed * yunuGI::Vector3(0.f, yawSign, 0.f).y;
									}
									if (eim.IsKeyboardDown(KeyCode::E))
									{
										positionDelta.y += speed * yunuGI::Vector3(0.f, yawSign, 0.f).y;
									}
									if (eim.IsKeyboardDown(KeyCode::S))
									{
										positionDelta.x -= speed * forwardDirection.x;
										positionDelta.z -= speed * forwardDirection.z;
									}
									if (eim.IsKeyboardDown(KeyCode::W))
									{
										positionDelta.x += speed * forwardDirection.x;
										positionDelta.z += speed * forwardDirection.z;
									}
									if (eim.IsKeyboardDown(KeyCode::A))
									{
										positionDelta.x -= speed * rightDirection.x;
										positionDelta.z -= speed * rightDirection.z;
									}
									if (eim.IsKeyboardDown(KeyCode::D))
									{
										positionDelta.x += speed * rightDirection.x;
										positionDelta.z += speed * rightDirection.z;
									}
									break;
								}
								default:
									break;
							}
							break;
						}
						case application::editor::CameraTypeState::Game:
						{
							// 동작하지 않음
							break;
						}
						default:
							break;
					}
				}
			}

			beforeMousePos = mousePos;

			if (moveFocus)
			{
				auto deltaPos2 = targetPos - editorCam->GetTransform()->GetWorldPosition();
				auto delTime = Time::GetDeltaTimeUnscaled();
				focalDelta.x += deltaPos2.x * focalSpeed * delTime;
				focalDelta.y += deltaPos2.y * focalSpeed * delTime;
				focalDelta.z += deltaPos2.z * focalSpeed * delTime;

				if (deltaPos2.MagnitudeSqr() <= 0.04)
				{
					moveFocus = false;
					focalDelta = yunuGI::Vector3();
				}
			}

			UpdateCameraView();
		}

		void EditorCamera::SetGamePerspective()
		{
			if (cameraPState == CameraPerspectiveState::Game)
			{
				return;
			}
			else
			{
				if (gameCam)
				{
					auto ts = editorCam->GetTransform();
					auto gcts = gameCam->GetTransform();
					auto pos = ts->GetWorldPosition();
					pos.y = gcts->GetWorldPosition().y;
					ts->SetWorldPosition(pos);
					position.y = pos.y;
					positionDelta = yunuGI::Vector3();

					auto rot = gcts->GetWorldRotation();
					ts->SetWorldRotation(rot);
					glm::quat glmRot = glm::quat(rot.w, -rot.x, -rot.y, rot.z);

					pitch = -glm::pitch(glmRot);
					yaw = -glm::yaw(glmRot);

					pitchDelta = 0.0f;
					yawDelta = 0.0f;

					forwardDirection = GetForwardDirection();
					rightDirection = GetRightDirection();

					editorCam->GetGI().SetVerticalFOV(gcFov);
					editorCam->GetGI().SetNear(gcNear);
					editorCam->GetGI().SetFar(gcFar);
					editorCam->GetGI().SetResolution(gcWidth, gcHeight);

					cameraPState = CameraPerspectiveState::Game;
				}
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
				editorCam->GetGI().SetVerticalFOV(ecFov);
				editorCam->GetGI().SetNear(ecNear);
				editorCam->GetGI().SetFar(ecFar);
				editorCam->GetGI().SetResolution(ecWidth, ecHeight);

				cameraPState = CameraPerspectiveState::Free;
				return;
			}
		}

		float EditorCamera::GetCameraSpeed() const
		{
			float speed = defaultMoveSpeed;
			if (eim.IsKeyboardDown(KeyCode::Control))
			{
				speed /= 2 - glm::log(defaultMoveSpeed);
			}

			if (eim.IsKeyboardDown(KeyCode::Shift))
			{
				speed *= 2 - glm::log(defaultMoveSpeed);
			}

			return glm::clamp(speed, min_Speed, max_Speed);
		}

		void EditorCamera::UpdateGI()
		{
			gcFov = gameCam->GetGI().GetVerticalFOV();
			gcNear = gameCam->GetGI().GetNear();
			gcFar = gameCam->GetGI().GetFar();
			gameCam->GetGI().GetResolution(&gcWidth, &gcHeight);

			if (cameraPState == CameraPerspectiveState::Game)
			{
				editorCam->GetGI().SetVerticalFOV(gcFov);
				editorCam->GetGI().SetNear(gcNear);
				editorCam->GetGI().SetFar(gcFar);
				editorCam->GetGI().SetResolution(gcWidth, gcHeight);
			}
		}

		void EditorCamera::OnPlayContents()
		{
			playCam = &RTSCam::Instance();
			PlayerController::Instance().SetCameraOffset();
		}

		void EditorCamera::OnPauseContents()
		{
			editorCam->SetCameraMain();
		}

		void EditorCamera::OnResumeContents()
		{
			playCam->SetCameraMain();
		}

		void EditorCamera::OnStopContents()
		{
			editorCam->SetCameraMain();
			playCam = nullptr;
		}

		void EditorCamera::ReloadGameCamera()
		{
			if (CameraManager::GetSingletonInstance().GetMainCam() != nullptr)
			{
				gameCam = CameraManager::GetSingletonInstance().GetMainCam()->GetCameraComponent();

				UpdateGI();

				if (cameraPState == CameraPerspectiveState::Game)
				{
					cameraPState = CameraPerspectiveState::Free;
					SetGamePerspective();
				}
			}
		}

		yunuGI::Vector3 EditorCamera::GetUpDirection() const
		{
			switch (cameraTState)
			{
				case application::editor::CameraTypeState::Editor:
				{
					auto up = editorCam->GetTransform()->GetWorldRotation().Up().Normalized();
					return yunuGI::Vector3(up.x, up.y, up.z);
				}
				case application::editor::CameraTypeState::Game:
				{
					auto up = gameCam->GetTransform()->GetWorldRotation().Up().Normalized();
					return yunuGI::Vector3(up.x, up.y, up.z);
				}
				default:
					break;
			}
			return yunuGI::Vector3();
		}

		yunuGI::Vector3 EditorCamera::GetRightDirection() const
		{
			switch (cameraTState)
			{
				case application::editor::CameraTypeState::Editor:
				{
					auto right = editorCam->GetTransform()->GetWorldRotation().Right().Normalized();
					return yunuGI::Vector3(right.x, right.y, right.z);
				}
				case application::editor::CameraTypeState::Game:
				{
					auto right = gameCam->GetTransform()->GetWorldRotation().Right().Normalized();
					return yunuGI::Vector3(right.x, right.y, right.z);
				}
				default:
					break;
			}
			return yunuGI::Vector3();
		}

		yunuGI::Vector3 EditorCamera::GetForwardDirection() const
		{
			switch (cameraTState)
			{
				case application::editor::CameraTypeState::Editor:
				{
					auto forward = editorCam->GetTransform()->GetWorldRotation().Forward().Normalized();
					return yunuGI::Vector3(forward.x, forward.y, forward.z);
				}
				case application::editor::CameraTypeState::Game:
				{
					auto forward = gameCam->GetTransform()->GetWorldRotation().Forward().Normalized();
					return yunuGI::Vector3(forward.x, forward.y, forward.z);
				}
				default:
					break;
			}
			return yunuGI::Vector3();
		}

		yunuGI::Vector3 EditorCamera::GetPosition() const
		{
			switch (cameraTState)
			{
				case application::editor::CameraTypeState::Editor:
				{
					auto pos = editorCam->GetTransform()->GetWorldPosition();
					return yunuGI::Vector3(pos.x, pos.y, pos.z);
				}
				case application::editor::CameraTypeState::Game:
				{
					auto pos = gameCam->GetTransform()->GetWorldPosition();
					return yunuGI::Vector3(pos.x, pos.y, pos.z);
				}
				default:
					break;
			}
			return yunuGI::Vector3();
		}

		yunuGI::Quaternion EditorCamera::GetOrientation() const
		{
			switch (cameraTState)
			{
				case application::editor::CameraTypeState::Editor:
				{
					auto rot = editorCam->GetTransform()->GetWorldRotation();
					return yunuGI::Quaternion(rot.x, rot.y, rot.z, rot.w);
				}
				case application::editor::CameraTypeState::Game:
				{
					auto rot = gameCam->GetTransform()->GetWorldRotation();
					return yunuGI::Quaternion(rot.x, rot.y, rot.z, rot.w);
				}
				default:
					break;
			}
			return yunuGI::Quaternion();
		}

		yunuGI::Vector3 EditorCamera::GetScale() const
		{
			switch (cameraTState)
			{
				case application::editor::CameraTypeState::Editor:
				{
					auto scale = editorCam->GetTransform()->GetWorldScale();
					return yunuGI::Vector3(scale.x, scale.y, scale.z);
				}
				case application::editor::CameraTypeState::Game:
				{
					auto scale = gameCam->GetTransform()->GetWorldScale();
					return yunuGI::Vector3(scale.x, scale.y, scale.z);
				}
				default:
					break;
			}
			return yunuGI::Vector3();
		}

		yunuGI::ICamera& EditorCamera::GetGI()
		{
			switch (cameraTState)
			{
				case application::editor::CameraTypeState::Editor:
					return editorCam->GetGI();
				case application::editor::CameraTypeState::Game:
					if (gameCam)
					{
						return gameCam->GetGI();
					}
					break;
				default:
					break;
			}
		}

		void EditorCamera::ChangeVTM(const yunuGI::Matrix4x4& vtm)
		{
			switch (cameraTState)
			{
				case application::editor::CameraTypeState::Editor:
				{
					auto gvtm = math::ConvertVTM(vtm);
					auto gwtm = glm::inverse(gvtm);

					glm::vec3 scale;
					glm::quat rotation;
					glm::vec3 position;
					glm::vec3 skew;
					glm::vec4 perspective;

					glm::decompose(gwtm, scale, rotation, position, skew, perspective);

					pitch = -glm::pitch(rotation);
					yaw = -glm::yaw(rotation);

					positionDelta = yunuGI::Vector3();

					pitchDelta = 0.0f;
					yawDelta = 0.0f;

					UpdateCameraView();
					break;
				}
				case application::editor::CameraTypeState::Game:
					break;
				default:
					break;
			}
		}

		void EditorCamera::UpdateCameraView()
		{
			glm::vec3 glmPos = glm::vec3(position.x, position.y, -position.z);

			//damping for smooth camera
			yawDelta *= 0.6f;
			pitchDelta *= 0.6f;
			positionDelta.x *= 0.8f;
			positionDelta.y *= 0.8f;
			positionDelta.z *= 0.8f;

			position.x += positionDelta.x + focalDelta.x;
			position.y += positionDelta.y + focalDelta.y;
			position.z += positionDelta.z + focalDelta.z;
			yaw += yawDelta;
			pitch += pitchDelta;

			if (editorCam)
			{
				editorCam->GetTransform()->SetWorldPosition(Vector3d(position.x, position.y, position.z));
				editorCam->GetTransform()->SetWorldRotation(Quaternion(Vector3d(pitch * yunutyEngine::math::Rad2Deg, yaw * yunutyEngine::math::Rad2Deg, 0.0)));

				forwardDirection = GetForwardDirection();
				rightDirection = GetRightDirection();
			}

			focalDelta = yunuGI::Vector3();

			UpdateGizmo();
		}

		void EditorCamera::UpdateGizmo()
		{

		}

		yunuGI::Matrix4x4 EditorCamera::GetWTM() const
		{
			switch (cameraTState)
			{
				case application::editor::CameraTypeState::Editor:
				{
					auto wm = editorCam->GetTransform()->GetWorldTM();
					return *reinterpret_cast<yunuGI::Matrix4x4*>(&wm);
				}
				case application::editor::CameraTypeState::Game:
				{
					if (gameCam)
					{
						auto wm = gameCam->GetTransform()->GetWorldTM();
						return *reinterpret_cast<yunuGI::Matrix4x4*>(&wm);
					}
				}
				default:
					return yunuGI::Matrix4x4();
			}
		}

		yunuGI::Matrix4x4 EditorCamera::GetVTM() const
		{
			return math::GetInverseMatrix(GetWTM());
		}

		yunuGI::Matrix4x4 EditorCamera::GetPTM() const
		{
			switch (cameraTState)
			{
				case application::editor::CameraTypeState::Editor:
				{
					float width;
					float height;
					editorCam->GetGI().GetResolution(&width, &height);
					DirectX::XMMATRIX pm = DirectX::XMMatrixPerspectiveFovLH(editorCam->GetGI().GetVerticalFOV(), width / height, editorCam->GetGI().GetNear(), editorCam->GetGI().GetFar());
					return *reinterpret_cast<yunuGI::Matrix4x4*>(&pm);
				}
				case application::editor::CameraTypeState::Game:
				{
					if (gameCam)
					{
						float width;
						float height;
						gameCam->GetGI().GetResolution(&width, &height);
						DirectX::XMMATRIX pm = DirectX::XMMatrixPerspectiveFovLH(gameCam->GetGI().GetVerticalFOV(), width / height, gameCam->GetGI().GetNear(), gameCam->GetGI().GetFar());
						return *reinterpret_cast<yunuGI::Matrix4x4*>(&pm);
					}
				}
				default:
					return yunuGI::Matrix4x4();
			}
		}
	}
}
