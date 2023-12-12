#pragma once
#include "YunutyEngine.h"
#include "DebugBeacon.h"
using namespace yunutyEngine;



class RTSCam :public yunutyEngine::graphics::Camera
{
	// 축과 각도로 쿼터니언 생성 함수
	Quaternion quaternionFromAxisAngle(Vector3d axis, float angle)
	{
		float halfAngle = angle * 0.5f;
		float s = std::sin(halfAngle);

		return Quaternion(std::cos(halfAngle), axis.x * s, axis.y * s, axis.z * s);
	}

    Quaternion RotateObjectByQuaternion(Quaternion objectQuaternion, Vector3d axisOfRotation, float rotationAngle) {
		// 축과 각도로 쿼터니언 생성
		Quaternion rotationQuaternion = quaternionFromAxisAngle(axisOfRotation, rotationAngle);

		// 현재의 쿼터니언을 객체의 회전에 적용
		objectQuaternion = Quaternion(
			objectQuaternion.w * rotationQuaternion.w - objectQuaternion.x * rotationQuaternion.x - objectQuaternion.y * rotationQuaternion.y - objectQuaternion.z * rotationQuaternion.z,
			objectQuaternion.w * rotationQuaternion.x + objectQuaternion.x * rotationQuaternion.w + objectQuaternion.y * rotationQuaternion.z - objectQuaternion.z * rotationQuaternion.y,
			objectQuaternion.w * rotationQuaternion.y - objectQuaternion.x * rotationQuaternion.z + objectQuaternion.y * rotationQuaternion.w + objectQuaternion.z * rotationQuaternion.x,
			objectQuaternion.w * rotationQuaternion.z + objectQuaternion.x * rotationQuaternion.y - objectQuaternion.y * rotationQuaternion.x + objectQuaternion.z * rotationQuaternion.w
		);

        return objectQuaternion;
		// 쿼터니언을 다시 Euler 각도로 변환
		//return Vector3d(objectQuaternion.x, objectQuaternion.y, objectQuaternion.z);
	}

	Quaternion lookAt(Vector3d source, Vector3d target, Vector3d up)
    {
        Vector3d forward = target - source;
		forward.Normalized();

        Vector3d right = Vector3d::Cross(up, forward);
		right.Normalized();

		up = Vector3d::Cross(forward, right);
		up.Normalized();

		// 회전 행렬을 생성
		float matrix[3][3] = {
			{right.x, up.x, -forward.x},
			{right.y, up.y, -forward.y},
			{right.z, up.z, -forward.z}
		};

		// 회전 행렬을 쿼터니언으로 변환
		float trace = matrix[0][0] + matrix[1][1] + matrix[2][2];

		float qw, qx, qy, qz;

		if (trace > 0) {
			float S = sqrt(trace + 1.0) * 2.0;
			qw = 0.25 * S;
			qx = (matrix[2][1] - matrix[1][2]) / S;
			qy = (matrix[0][2] - matrix[2][0]) / S;
			qz = (matrix[1][0] - matrix[0][1]) / S;
		}
		else if ((matrix[0][0] > matrix[1][1]) && (matrix[0][0] > matrix[2][2])) {
			float S = sqrt(1.0 + matrix[0][0] - matrix[1][1] - matrix[2][2]) * 2.0;
			qw = (matrix[2][1] - matrix[1][2]) / S;
			qx = 0.25 * S;
			qy = (matrix[0][1] + matrix[1][0]) / S;
			qz = (matrix[0][2] + matrix[2][0]) / S;
		}
		else if (matrix[1][1] > matrix[2][2]) {
			float S = sqrt(1.0 + matrix[1][1] - matrix[0][0] - matrix[2][2]) * 2.0;
			qw = (matrix[0][2] - matrix[2][0]) / S;
			qx = (matrix[0][1] + matrix[1][0]) / S;
			qy = 0.25 * S;
			qz = (matrix[1][2] + matrix[2][1]) / S;
		}
		else {
			float S = sqrt(1.0 + matrix[2][2] - matrix[0][0] - matrix[1][1]) * 2.0;
			qw = (matrix[1][0] - matrix[0][1]) / S;
			qx = (matrix[0][2] + matrix[2][0]) / S;
			qy = (matrix[1][2] + matrix[2][1]) / S;
			qz = 0.25 * S;
		}

		return Quaternion(qw, qx, qy, qz);
	}


public:
    bool roamingMode = false;
    function<void(Vector3d)> groundLeftClickCallback{[](Vector3d) {}};
    function<void(Vector3d)> groundRightClickCallback{[](Vector3d) {}};
    function<void(Vector3d)> groundHoveringClickCallback{[](Vector3d) {}};

    void Start()
    {
        //Vector3d startPosition = Vector3d(-30, 30, 0);
        //GetTransform()->SetWorldPosition(startPosition);
        //GetTransform()->rotation = lookAt(startPosition, Vector3d::zero, GetTransform()->rotation.Up());
    }

    void Update()
    {
        if (Input::isKeyPushed(KeyCode::Tab))
            roamingMode = !roamingMode;

        if (!roamingMode)
        {
            float cameraSpeed = 50.f;
            Camera::Update();
            Vector3d deltaDirection = Vector3d::zero;

            if (yunutyEngine::Input::isKeyDown(KeyCode::LShift))
                cameraSpeed = 20.0f;

            auto lastMousePos = yunutyEngine::Input::getMouseScreenPositionNormalized();
            if (yunutyEngine::Input::isKeyDown(KeyCode::A) || lastMousePos.x <= 0)
                deltaDirection -= Vector3d::right;
            if (yunutyEngine::Input::isKeyDown(KeyCode::D) || lastMousePos.x >= 1)
                deltaDirection += Vector3d::right;
            if (yunutyEngine::Input::isKeyDown(KeyCode::W) || lastMousePos.y <= 0)
                deltaDirection += Vector3d::forward;
            if (yunutyEngine::Input::isKeyDown(KeyCode::S) || lastMousePos.y >= 1)
                deltaDirection -= Vector3d::forward;

            GetTransform()->position += deltaDirection.Normalized() * Time::GetDeltaTime() * cameraSpeed;
            Quaternion quat = Quaternion::MakeWithForwardUp(Vector3d::down,Vector3d::forward);
            GetTransform()->rotation = quat;
        }
        else
        {
            float cameraSpeed = 1.0f;
            Camera::Update();
            Vector3d deltaPosition = Vector3d::zero;

            if (yunutyEngine::Input::isKeyDown(KeyCode::LShift))
                cameraSpeed = 20.0f;

            if (yunutyEngine::Input::isKeyDown(KeyCode::A))
                deltaPosition -= GetTransform()->GetWorldRotation().Right();
            if (yunutyEngine::Input::isKeyDown(KeyCode::D))
                deltaPosition += GetTransform()->GetWorldRotation().Right();
            if (yunutyEngine::Input::isKeyDown(KeyCode::W))
                deltaPosition += GetTransform()->GetWorldRotation().Forward();
            if (yunutyEngine::Input::isKeyDown(KeyCode::S))
                deltaPosition -= GetTransform()->GetWorldRotation().Forward();
            if (yunutyEngine::Input::isKeyDown(KeyCode::Q))
                deltaPosition -= Vector3d::up;
            if (yunutyEngine::Input::isKeyDown(KeyCode::E))
                deltaPosition += Vector3d::up;
            static auto lastMousePos = yunutyEngine::Input::getMouseScreenPosition();

            auto dMousePos = yunutyEngine::Input::getMouseScreenPosition() - lastMousePos;
            lastMousePos = yunutyEngine::Input::getMouseScreenPosition();

            if (yunutyEngine::Input::isKeyDown(KeyCode::MouseRightClick))
            {
                constexpr float rotationSpeedFactor = 0.0003f;
                //auto euler = GetTransform()->rotation.Euler();
                //euler.y += dMousePos.x * rotationSpeedFactor;
                //euler.x += dMousePos.y * rotationSpeedFactor;
                Quaternion& rot = GetTransform()->rotation;
                //Vector3d newForward =
                //    rot.Forward() +
                //    dMousePos.x * rotationSpeedFactor * rot.Right() +
                //    -dMousePos.y * rotationSpeedFactor * rot.Up();
                ////Vector3d newRight =
                ////    //Vector3d::right +
                ////    rot.Right() +
                ////    dMousePos.x * rotationSpeedFactor * -rot.Forward();
                //Vector3d newUp = rot.Up() + -dMousePos.y * rotationSpeedFactor * -rot.Forward();
                //Vector3d newRight = Vector3d::Cross(newUp, newForward);
                ////GetTransform()->rotation = Quaternion::MakeWithAxes(euler);
                //GetTransform()->rotation = Quaternion::MakeWithAxes(newRight, newUp, newForward);

                Vector3d temp = rot.Up();
                GetTransform()->rotation = RotateObjectByQuaternion(GetTransform()->GetWorldRotation(), Vector3d::up, rotationSpeedFactor * dMousePos.x);
                GetTransform()->rotation = RotateObjectByQuaternion(GetTransform()->GetWorldRotation(), Vector3d::right, rotationSpeedFactor * dMousePos.y);


            }

            GetTransform()->position += deltaPosition.Normalized() * Time::GetDeltaTime() * cameraSpeed;
        }

        if (!roamingMode)
        {
            Vector3d projectedPoint;
            auto resolution = graphics::Renderer::SingleInstance().GetResolution();
            auto centeredPosition = Input::getMouseScreenPositionNormalized();
            centeredPosition.x -= 0.5;
            centeredPosition.y -= 0.5;
            centeredPosition.y *= -1;
            // 카메라 해상도가 1280, 800일때 near plane의 가로가 1.28, 0.8임.
            auto forward = GetTransform()->GetWorldRotation().Forward();
            auto right = GetTransform()->GetWorldRotation().Right();
            auto up = GetTransform()->GetWorldRotation().Up();
            projectedPoint = GetTransform()->GetWorldPosition() +
                forward * expectedPlaneDistance() +
                right * centeredPosition.x * 0.001 * resolution.x * expectedPlaneDistance() +
                up * centeredPosition.y * 0.001 * resolution.y * expectedPlaneDistance();

            if (Input::isKeyPushed(KeyCode::MouseLeftClick) || Input::isKeyPushed(KeyCode::MouseRightClick))
                DebugBeacon::PlaceBeacon(projectedPoint, Input::isKeyPushed(KeyCode::MouseLeftClick) ?
                    yunuGI::Color::red() : yunuGI::Color::blue(), { 0.2,0.2,0.2 });
            if (Input::isKeyPushed(KeyCode::MouseLeftClick))
                groundLeftClickCallback(projectedPoint);
            else if (Input::isKeyPushed(KeyCode::MouseRightClick))
                groundRightClickCallback(projectedPoint);

            groundHoveringClickCallback(projectedPoint);
        }
    }
private:
    float expectedPlaneDistance() { return abs(GetTransform()->GetWorldPosition().y); };
};

