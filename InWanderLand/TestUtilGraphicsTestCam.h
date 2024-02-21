#pragma once
#include "YunutyEngine.h"
#include "DebugBeacon.h"

#include <algorithm>

using namespace yunutyEngine;
namespace tests
{
	class GraphicsTestCam :public yunutyEngine::graphics::Camera
	{
	public:
		function<void(Vector3d)> groundLeftClickCallback{ [](Vector3d) {} };
		function<void(Vector3d)> groundLeftClickReleaseCallback{ [](Vector3d) {} };
		function<void(Vector3d)> groundRightClickCallback{ [](Vector3d) {} };
		function<void(Vector3d)> groundRightClickReleaseCallback{ [](Vector3d) {} };
		function<void(Vector3d)> groundHoveringClickCallback{ [](Vector3d) {} };

		virtual void Start() override
		{
			pitch = GetTransform()->GetWorldRotation().Euler().x;
			yaw = GetTransform()->GetWorldRotation().Euler().y;
			inputUpdate = true;
		}

		virtual void Update() override
		{
			Camera::Update();
			mousePos = Input::getMouseScreenPosition();

			Vector2d deltaPos = (mousePos - beforeMousePos) * 0.002;

			if (inputUpdate)
			{
				if (Input::isKeyDown(KeyCode::MouseRightClick))
				{
					const double yawSign = GetTransform()->GetWorldRotation().Up().y < 0 ? -1.0 : 1.0;
					const double speed = GetCameraSpeed();

					Vector3d forwardDirection = GetTransform()->GetWorldRotation().Forward();
					Vector3d rightDirection = GetTransform()->GetWorldRotation().Right();

					if (Input::isKeyDown(KeyCode::Q))
					{
						positionDelta.y -= speed * Vector3d(0.0, yawSign, 0.0).y;
					}
					if (Input::isKeyDown(KeyCode::Alphabet_E))
					{
						positionDelta.y += speed * Vector3d(0.0, yawSign, 0.0).y;
					}
					if (Input::isKeyDown(KeyCode::S))
					{
						positionDelta -= speed * forwardDirection;
					}
					if (Input::isKeyDown(KeyCode::W))
					{
						positionDelta += speed * forwardDirection;
					}
					if (Input::isKeyDown(KeyCode::A))
					{
						positionDelta -= speed * rightDirection;
					}
					if (Input::isKeyDown(KeyCode::D))
					{
						positionDelta += speed * rightDirection;
					}

					double maxRate = 1.2;
					yawDelta += clamp(yawSign * deltaPos.x * rotationSpeed, -maxRate, maxRate);
					pitchDelta += clamp(deltaPos.y * rotationSpeed, -maxRate, maxRate);
				}

				Vector3d front = GetTransform()->GetWorldRotation().Forward();
				auto distToXZPlane = abs(GetTransform()->GetWorldPosition().y);
				auto centeredPosition = Input::getMouseScreenPositionNormalized();
				centeredPosition.x -= 0.5;
				centeredPosition.y -= 0.5;
				centeredPosition.y *= -1;
				Vector3d projectedPoint = GetProjectedPoint(centeredPosition, distToXZPlane, Vector3d(0, 1, 0));

				//if (Input::isKeyPushed(KeyCode::MouseLeftClick) || Input::isKeyPushed(KeyCode::MouseRightClick))
				//	DebugBeacon::PlaceBeacon(projectedPoint, Input::isKeyPushed(KeyCode::MouseLeftClick) ?
				//		yunuGI::Color::red() : yunuGI::Color::blue(), { 0.2,0.2,0.2 });
				if (Input::isKeyPushed(KeyCode::MouseLeftClick))
				{
					if (groundLeftClickCallback) groundLeftClickCallback(projectedPoint);
				}
				else if (Input::isKeyLifted(KeyCode::MouseLeftClick))
				{
					if (groundLeftClickReleaseCallback) groundLeftClickReleaseCallback(projectedPoint);
				}
				if (Input::isKeyPushed(KeyCode::MouseRightClick))
				{
					if (groundRightClickCallback)
					{
						groundRightClickCallback(projectedPoint);
					}
				}
				else if (Input::isKeyLifted(KeyCode::MouseRightClick))
				{
					if (groundRightClickReleaseCallback) groundRightClickReleaseCallback(projectedPoint);
				}

				groundHoveringClickCallback(projectedPoint);
			}

			beforeMousePos = mousePos;

			UpdateCameraView();
		}
		float expectedPlaneDistance() { return abs(GetTransform()->GetWorldPosition().y); };

		float GetCameraSpeed() const
		{
			double speed = defaultMoveSpeed;
			if (Input::isKeyDown(KeyCode::Control))
			{
				speed /= 2 - log(defaultMoveSpeed);
			}

			if (Input::isKeyDown(KeyCode::LShift))
			{
				speed *= 2 - log(defaultMoveSpeed);
			}

			return clamp(speed, min_Speed, max_Speed);
		}

		bool SetInputUpdate(bool inputUpdate) { this->inputUpdate = inputUpdate; }

	private:
		void UpdateCameraView()
		{
			//damping for smooth camera
			yawDelta *= 0.6;
			pitchDelta *= 0.6;
			positionDelta.x *= 0.8;
			positionDelta.y *= 0.8;
			positionDelta.z *= 0.8;

			Vector3d pos = GetTransform()->GetWorldPosition();
			GetTransform()->SetWorldPosition(pos + positionDelta);

			yaw += yawDelta;
			pitch += pitchDelta;
			GetTransform()->SetWorldRotation(Quaternion(Vector3d(pitch,yaw,0.0)));
		}

		bool inputUpdate = false;

		Vector3d positionDelta = Vector3d();

		Vector2d mousePos = Vector2d();
		Vector2d beforeMousePos = Vector2d();

		double pitch = 0.0;
		double pitchDelta = 0.0;
		double yaw = 0.0;
		double yawDelta = 0.0;

		double min_Speed = 0.002;
		double max_Speed = 10.0;
		double defaultMoveSpeed = 0.008;
		double rotationSpeed = 10.0;
	};
}

