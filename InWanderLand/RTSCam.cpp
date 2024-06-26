#include "InWanderLand.h"
#include "RTSCam.h"
#include "RegionData.h"
#include "eKeyCode.h"

using namespace yunutyEngine::math;
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
        float camInterpolationSpeed = GlobalConstant::GetSingletonInstance().pod.cameraInterpolationSpeed;
        GetTransform()->SetWorldPosition(GetTransform()->GetWorldPosition() + deltaPos * camInterpolationSpeed * Time::GetDeltaTimeUnscaled() + camOffsetAdder);
        GetTransform()->SetWorldRotation(Quaternion::Lerp(GetTransform()->GetWorldRotation(), GetIdealRotation(), camInterpolationSpeed * Time::GetDeltaTimeUnscaled()));
    }
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
void RTSCam::SetIdealPosition(const Vector3d& position)
{
    idealPosition = position;
}
void RTSCam::SetIdealRotation(const Quaternion& rotation)
{
    idealRotation = rotation;
}
Vector3d RTSCam::GetIdealPosition()
{
    return idealPosition;
}
Quaternion RTSCam::GetIdealRotation()
{
    return idealRotation;
}

void RTSCam::OnContentsPlay()
{
    pitch = 0.0;
    pitchDelta = 0.0;
    yaw = 0.0;
    yawDelta = 0.0;
    positionDelta = Vector3d::zero;

    SetActive(true);
    Start();
}

void RTSCam::OnContentsStop()
{
    for (auto& each : shakeCoroutines)
        DeleteCoroutine(each);

    shakeCoroutines.clear();

    SetActive(false);
}

void RTSCam::ApplyShake(float shakeDistance, float frequency, float decreaseFactor, const Vector3d& origin)
{
    shakeCoroutines.push_back(StartCoroutine(ShakeCoroutine(shakeDistance, frequency, decreaseFactor, origin)));
}

coroutine::Coroutine RTSCam::ShakeCoroutine(float shakeDistance, float frequency, float decreaseFactor, Vector3d origin)
{
    Vector3d direction = (GetTransform()->GetWorldPosition() - origin).Normalized();
    direction = direction.Normalized();
    static constexpr float epsilon = 0.01f;
    auto offset = camOffsetAdder.AcquireFactor();
    while (std::fabsf(shakeDistance) > epsilon)
    {
        coroutine::ForSeconds forSeconds{ 1.0f / frequency };
        while (forSeconds.Tick())
        {
            *offset = forSeconds.ElapsedNormalized() * shakeDistance * direction;
            co_await std::suspend_always{};
        }
        shakeDistance *= -decreaseFactor;
    }
    co_return;
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

