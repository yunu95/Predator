#include "InWanderLand.h"
#include "EditorCamera.h"
#include "EditorMath.h"
#include "MouseEvents.h"

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
		float EditorCamera::min_Speed = 0.002f;
		float EditorCamera::max_Speed = 2.0f;

		void EditorCamera::Initialize(yunutyEngine::graphics::Camera* gameCam)
		{
			// Game 카메라 세팅
			this->gameCam = gameCam;


			// Editor 카메라를 Scene에 생성하고 해당 카메라를 메인으로 변경
			auto scene = yunutyEngine::Scene::getCurrentScene();
			auto ecc = scene->AddGameObject()->AddComponent<EditorCameraComponent>();
			ecc->SetCameraMain();
			auto ts = ecc->GetTransform();
			if (gameCam)
			{
				auto gcts = gameCam->GetTransform();
				ts->SetWorldPosition(gcts->GetWorldPosition());
				ts->SetWorldRotation(gcts->GetWorldRotation());
				cameraPState = CameraPerspectiveState::Game;
			}
			else
			{
				ts->SetWorldPosition(Vector3d{ 0, 10, 0 });
				ts->SetWorldRotation(Vector3d{ 60, 0, 0 });
				cameraPState = CameraPerspectiveState::Free;
			}

			editorCam = ecc;
			cameraTState = CameraTypeState::Editor;

			auto ecPos = ts->GetWorldPosition();
			position.x = ecPos.x;
			position.y = ecPos.y;
			position.z = ecPos.z;

			auto rot = ts->GetWorldRotation();
			glm::quat glmRot = glm::quat(rot.w, rot.x, rot.y, -rot.z);

			pitch = glm::pitch(glmRot);
			yaw = glm::yaw(glmRot);

			const auto orientation = GetOrientation();
			const glm::quat glmOrientation = glm::quat{ (float)orientation.w, (float)orientation.x, (float)orientation.y, -(float)orientation.z };
			Vector3f fd = ts->GetWorldRotation().Forward();
			forwardDirection = *reinterpret_cast<yunuGI::Vector3*>(&fd);
			viewMatrix = math::ConvertMatrix4x4(glm::inverse(glm::translate(glm::mat4(1.0f), glm::vec3{ position.x, position.y, -position.z }) * glm::toMat4(glmOrientation)));
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
						cameraTState = CameraTypeState::Game;
						gameCam->SetCameraMain();
					}
					break;
				}
				case application::editor::CameraTypeState::Game:
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
			yunuGI::Vector2 deltaPos;
			deltaPos.x = (mousePos.x - beforeMousePos.x) * 0.002f;
			deltaPos.y = (mousePos.y - beforeMousePos.y) * 0.002f;

			if (inputUpdate)
			{
				if (eim.IsKeyboardUp(KeyCode::C))
				{
					SwitchCam();
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

							const float distance = glm::distance(glm::vec3{ focalPos.x, focalPos.y, -focalPos.z }, glm::vec3{ position.x, position.y, -position.z });
							focalPos.x = position.x + GetForwardDirection().x * distance;
							focalPos.y = position.y + GetForwardDirection().y * distance;
							focalPos.z = position.z + GetForwardDirection().z * distance;
							focalDistance = distance;
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
			position.x += positionDelta.x;
			position.y += positionDelta.y;
			position.z += positionDelta.z;
			yaw += yawDelta;
			pitch += pitchDelta;

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
					glm::quat glmRot = glm::quat(rot.w, rot.x, rot.y, -rot.z);

					pitch = glm::pitch(glmRot);
					yaw = glm::yaw(glmRot);

					pitchDelta = 0.0f;
					yawDelta = 0.0f;

					const auto orientation = GetOrientation();
					const glm::quat glmOrientation = glm::quat{ (float)orientation.w, (float)orientation.x, (float)orientation.y, -(float)orientation.z };
					Vector3f fd = ts->GetWorldRotation().Forward();
					forwardDirection = *reinterpret_cast<yunuGI::Vector3*>(&fd);
					viewMatrix = math::ConvertMatrix4x4(glm::inverse(glm::translate(glm::mat4(1.0f), glm::vec3{ position.x, position.y, -position.z }) * glm::toMat4(glmOrientation)));

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

		yunuGI::Vector3 EditorCamera::GetUpDirection() const
		{
			return math::RotateVector3(yunuGI::Vector3(0.0f, 1.0f, 0.0f), math::GetRotationMatrix(GetOrientation()));
		}

		yunuGI::Vector3 EditorCamera::GetRightDirection() const
		{
			return math::RotateVector3(yunuGI::Vector3(1.0f, 0.0f, 0.0f), math::GetRotationMatrix(GetOrientation()));
		}

		yunuGI::Vector3 EditorCamera::GetForwardDirection() const
		{
			return math::RotateVector3(yunuGI::Vector3(0.0f, 0.0f, 1.0f), math::GetRotationMatrix(GetOrientation()));
		}

		yunuGI::Quaternion EditorCamera::GetOrientation() const
		{
			return math::ConvertQuaternion(glm::quat(glm::vec3(pitch + pitchDelta, yaw + yawDelta, 0.0f)));
		}

		void EditorCamera::UpdateCameraView()
		{
			glm::vec3 glmPos = glm::vec3(position.x, position.y, -position.z);

			const float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;

			// Extra step to handle the problem when the camera direction is the same as the up vector
			auto beforeFD = GetForwardDirection();
			auto beforeUD = GetUpDirection();
			const float cosAngle = glm::dot(glm::vec3{ beforeFD.x, beforeFD.y, -beforeFD.z }, glm::vec3{ beforeUD.x, beforeUD.y, -beforeUD.z });
			if (cosAngle * yawSign > 0.99f)
				pitchDelta = 0.f;

			auto newFD = GetForwardDirection();
			const glm::vec3 lookAt{ position.x + newFD.x, position.y + newFD.y, -position.z - newFD.z };
			forwardDirection = math::ConvertVector3(glm::normalize(lookAt - glmPos));
			rightDirection = GetRightDirection();
			focalDistance = glm::distance(glmPos, glm::vec3{ focalPos.x, focalPos.y, -focalPos.z });
			viewMatrix = math::ConvertMatrix4x4(glm::lookAt(glmPos, lookAt, glm::vec3{ 0.f, yawSign, 0.f }));

			//damping for smooth camera
			yawDelta *= 0.6f;
			pitchDelta *= 0.6f;
			positionDelta.x *= 0.8f;
			positionDelta.y *= 0.8f;
			positionDelta.z *= 0.8f;

			if (editorCam)
			{
				editorCam->GetTransform()->SetWorldPosition(Vector3d(position.x, position.y, position.z));
				auto orientation = GetOrientation();
				editorCam->GetTransform()->SetWorldRotation(Quaternion(orientation.w, orientation.x, orientation.y, orientation.z));
			}
		}
	}
}
