#include "InWanderLand.h"
#include "RTSCam.h"
#include "RegionData.h"
#include "eKeyCode.h"

void RTSCam::Start()
{
    //Quaternion quat = Quaternion(Vector3d(60, 0, 0));
    //GetTransform()->SetLocalRotation(quat);

    /// 카메라가 초기에 캐릭터 상단에서 아래쪽을 바라보는 식으로
    /// 구성되어 있다고 가정합니다.
    float dis = 0;
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
            SetTarget(each);
            break;
        }
    }

    auto& gi = GetGI();

    ingameInitRot = rot;
    ingameInitScale = GetTransform()->GetWorldScale();
    initFov = gi.GetVerticalFOV();
    initNear = gi.GetNear();
    initFar = gi.GetFar();
    gi.GetResolution(&initWidth, &initHeight);
}

void RTSCam::Update()
{
    if (!updateability)
    {
        return;
    }

#ifdef EDITOR
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
#endif
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

    // 디버그용 자유 카메라
    if (roamingMode)
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
    // 인게임에 적용되는 카메라
    else
    {
        // targetPos를 적절하게 지정하기만 하면 그 targetPos를 따라 카메라가 스무스하게 따라갑니다.
        Vector3d targetPos{ GetIdealPosition() };

        // 카메라를 스무스하게 이동시키는 부분
        auto deltaPos = targetPos - GetTransform()->GetWorldPosition();
        static constexpr float followRate = 3.9;
        GetTransform()->SetWorldPosition(GetTransform()->GetWorldPosition() + deltaPos * followRate * Time::GetDeltaTimeUnscaled());

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
void RTSCam::UnConstrainRegion()
{
    ConstrainByRegion(nullptr);
}
void RTSCam::ConstrainByRegion(const application::editor::RegionData* region)
{
    contrainingRegion = region;
};
void RTSCam::SetTarget(GameObject* target)
{
    SetTargets({ target });
}
void RTSCam::SetTargets(const std::vector<GameObject*>& targets)
{
    this->targets = targets;
}
float RTSCam::GetCameraSpeed() const
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
Vector3d RTSCam::GetIdealPosition()
{
    Vector3d targetPos = GetTransform()->GetWorldPosition();
    // 영웅이 선택되어 있고, 카메라가 선택된 영웅을 따라가는 경우 targetPos는 영웅의 위치로 설정됩니다.
    if (!targets.empty())
    {
        targetPos = Vector3d::zero;
        for (auto each : targets)
        {
            targetPos += each->GetTransform()->GetWorldPosition();
        }
        targetPos /= targets.size();
        if (targets.size() >= 2)
        {
            targetPos += distance * mulTargetsDistanceFactor;
        }
        else
        {
            targetPos += distance;
        }
    }
    // 카메라가 지역 제한에 걸렸을 경우, targetPos를 지역 안으로 정의합니다.
    if (contrainingRegion)
    {
        targetPos.x = clamp(targetPos.x, distance.x + contrainingRegion->pod.x - contrainingRegion->pod.width * 0.5, distance.x + contrainingRegion->pod.x + contrainingRegion->pod.width * 0.5);
        targetPos.z = clamp(targetPos.z, distance.z + contrainingRegion->pod.z - contrainingRegion->pod.height * 0.5, distance.z + contrainingRegion->pod.z + contrainingRegion->pod.height * 0.5);
    }
    return targetPos;
}

void RTSCam::PlayFunction()
{

}

void RTSCam::StopFunction()
{
	if (GetGameObject()->GetSelfActive())
		GetGameObject()->SetSelfActive(false);
}

void RTSCam::UpdateCameraView()
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

