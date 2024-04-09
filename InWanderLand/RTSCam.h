#pragma once
#include "YunutyEngine.h"
#include "DebugBeacon.h"
#include "RegionData.h"
#include "Unit.h"
#include "UIManager.h"
using namespace yunutyEngine;

class RTSCam :public yunutyEngine::graphics::Camera
{
public:
	bool roamingMode = false;
	function<void(Vector3d)> groundLeftClickCallback{ [](Vector3d) {} };
	function<void(Vector3d)> groundLeftClickReleaseCallback{ [](Vector3d) {} };
	function<void(Vector3d)> groundRightClickCallback{ [](Vector3d) {} };
	function<void(Vector3d)> groundRightClickReleaseCallback{ [](Vector3d) {} };
	function<void(Vector3d)> groundHoveringClickCallback{ [](Vector3d) {} };
	function<void()> deleteButtonCallback{ []() {} };
	function<void()> tabButtonCallback{ []() {} };
	function<void()> xButtonCallback{ []() {} };

	virtual void Start()
	{
		//Quaternion quat = Quaternion(Vector3d(60, 0, 0));
		//GetTransform()->SetLocalRotation(quat);

		/// 카메라가 초기에 캐릭터 상단에서 아래쪽을 바라보는 식으로
		/// 구성되어 있다고 가정합니다.
		float dis = 10;
		auto forward = GetTransform()->GetWorldRotation().Forward();
		auto coeff = abs(forward.y);
		coeff = GetTransform()->GetWorldPosition().y / coeff;
		auto minus = Vector3d(forward.x, 0, forward.z);
		distance = -(forward * coeff - (minus * dis));
		rot = GetTransform()->GetWorldRotation();

		pitch = rot.Euler().x * yunutyEngine::math::Deg2Rad;
		yaw = rot.Euler().y * yunutyEngine::math::Deg2Rad;

		for (auto& each : Scene::getCurrentScene()->GetChildren())
		{
			auto unit = each->GetComponent<Unit>();
			if (unit && unit->GetUnitSide() == Unit::UnitSide::Player)
			{
				target = each;
				break;
			}
		}
	}

	void Update()
	{
		if (Input::isKeyPushed(KeyCode::Tab))
		{
			roamingMode = !roamingMode;
			GetTransform()->SetWorldRotation(rot);
			pitch = rot.Euler().x * yunutyEngine::math::Deg2Rad;
			yaw = rot.Euler().y * yunutyEngine::math::Deg2Rad;
			pitchDelta = 0.0;
			yawDelta = 0.0;
			positionDelta = Vector3d();
		}

		if (yunutyEngine::Input::isKeyPushed(KeyCode::Delete))
			deleteButtonCallback();

		if (yunutyEngine::Input::isKeyPushed(KeyCode::Tab))
			tabButtonCallback();

		if (yunutyEngine::Input::isKeyPushed(KeyCode::X))
			xButtonCallback();

		// 디버그 그래픽스 키고 끄기용
		if (Input::isKeyPushed(yunutyEngine::KeyCode::BackSlash))
		{
			DebugGraphic::SetDebugGraphicsEnabled(!DebugGraphic::AreDebugGraphicsEnabled());
		}

		if (!roamingMode)
		{
			if (target)
			{
				GetTransform()->SetWorldPosition(target->GetTransform()->GetWorldPosition() + distance);
			}
		}
		else
		{
			mousePos = Input::getMouseScreenPosition();

			Vector2d deltaPos = (mousePos - beforeMousePos) * 0.002;

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

				double maxRate = 0.12f;
				yawDelta += clamp(yawSign * deltaPos.x * rotationSpeed, -maxRate, maxRate);
				pitchDelta += clamp(deltaPos.y * rotationSpeed, -maxRate, maxRate);
			}

			beforeMousePos = mousePos;

			UpdateCameraView();
		}

		if (!roamingMode && !UIManager::Instance().IsMouseOnButton())
		{
			auto front = yunutyEngine::graphics::Camera::GetMainCamera()->GetTransform()->GetWorldRotation().Forward();
			auto distToXZPlane = abs(yunutyEngine::graphics::Camera::GetMainCamera()->GetTransform()->GetWorldPosition().y);
			auto centeredPosition = Input::getMouseScreenPositionNormalized();
			centeredPosition.x -= 0.5;
			centeredPosition.y -= 0.5;
			centeredPosition.y *= -1;
			auto projectedPoint = yunutyEngine::graphics::Camera::GetMainCamera()->GetProjectedPoint(centeredPosition, distToXZPlane, Vector3d(0, 1, 0));

			if (Input::isKeyPushed(KeyCode::MouseLeftClick) || Input::isKeyPushed(KeyCode::MouseRightClick))
				DebugBeacon::PlaceBeacon(projectedPoint, Input::isKeyPushed(KeyCode::MouseLeftClick) ?
					yunuGI::Color::red() : yunuGI::Color::blue(), { 0.2,0.2,0.2 });
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
				if (groundRightClickCallback) groundRightClickCallback(projectedPoint);
			}
			else if (Input::isKeyLifted(KeyCode::MouseRightClick))
			{
				if (groundRightClickReleaseCallback) groundRightClickReleaseCallback(projectedPoint);
			}

			groundHoveringClickCallback(projectedPoint);
		}
	}
	void ConstrainByRegion(const application::editor::RegionData* region)
	{
	};

	void SetTarget(GameObject* target)
	{
		this->target = target;
	}

	float GetCameraSpeed() const
	{
		float speed = defaultMoveSpeed;
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
		GetTransform()->SetWorldRotation(Quaternion(Vector3d(pitch * yunutyEngine::math::Rad2Deg, yaw * yunutyEngine::math::Rad2Deg, 0.0)));
	}

	const application::editor::RegionData* contrainingRegion;
	Vector3d euler;
	GameObject* target = nullptr;
	Vector3d distance;

	Vector3d positionDelta = Vector3d();

	Vector2d mousePos = Vector2d();
	Vector2d beforeMousePos = Vector2d();

	Quaternion rot;

	float pitch = 0.0;
	float pitchDelta = 0.0;
	float yaw = 0.0;
	float yawDelta = 0.0;

	float min_Speed = 0.002;
	float max_Speed = 2.0;
	float defaultMoveSpeed = 0.03;
	float rotationSpeed = 0.3;

};

