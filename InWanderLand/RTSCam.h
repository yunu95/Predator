#pragma once
#include "YunutyEngine.h"
#include "DebugBeacon.h"
#include "Unit.h"
#include "UIManager.h"
#include "ContentsObservee.h"
#include "SingletonComponent.h"
#include "Adder.h"
#include "ProgressManager.h"
#include "ProgressTracker.h"

using namespace yunutyEngine;

namespace application
{
    namespace editor
    {
        class RegionData;
    }
}

class RTSCam : public yunutyEngine::graphics::Camera, public SingletonComponent<RTSCam>, public PermanentObservee, public application::ProgressTracker
{
public:
    // 카메라가 타겟팅하는 게임 오브젝트가 여러개일 경우 distance에 곱해질 스케일러
    static constexpr float mulTargetsDistanceFactor = 1.2;
    bool roamingMode = false;
    function<void(Vector3d)> groundLeftClickCallback{ [](Vector3d) {} };
    function<void(Vector3d)> groundLeftClickReleaseCallback{ [](Vector3d) {} };
    function<void(Vector3d)> groundRightClickCallback{ [](Vector3d) {} };
    function<void(Vector3d)> groundRightClickReleaseCallback{ [](Vector3d) {} };
    function<void(Vector3d)> groundHoveringClickCallback{ [](Vector3d) {} };
    function<void()> deleteButtonCallback{ []() {} };
    function<void()> tabButtonCallback{ []() {} };
    function<void()> xButtonCallback{ []() {} };

    virtual void Start();
    void Update();
    float GetCameraSpeed() const;

    void SetUpdateability(bool updateability) { this->updateability = updateability; }
    bool GetUpdateability() { return updateability; }
    void SetIdealPosition(const Vector3d& position);
    void SetIdealRotation(const Quaternion& rotation);
    Vector3d GetIdealPosition();
    Quaternion GetIdealRotation();
    Quaternion GetInitRotation() { return ingameInitRot; }
    Vector3d GetInitScale() { return ingameInitScale; }
    float GetInitVerticalFOV() { return initFov; }
    float GetInitNear() { return initNear; }
    float GetInitFar() { return initFar; }
    float GetInitWidth() { return initWidth; }
    float GetInitHeight() { return initHeight; }
    virtual Component* GetComponent() override { return this; }

    virtual void OnContentsPlay() override;
    virtual void OnContentsStop() override;
    // 초당 frequency만큼 진동하는 카메라 흔들림을 적용합니다.
    // 카메라의 흔들림 강도는 진원지로부터의 거리의 제곱의 역수에 비례합니다.
    void ApplyShake(float shakeDistance, float frequency, float decreaseFactor, const Vector3d& origin);

    // Game 을 재시작하게 될 경우,
    // 초기화 세팅을 하기 위한 함수입니다.
    virtual void ProgressInitialize() override;

    // Game 진행 도중에 호출되어 현재의 상태를 저장하여
    // 추후에 Recovery 에서 사용할 수 있도록 개별적으로 적합한
    // Save Data 를 생성하는 함수입니다.
    virtual void CurrentProgressSave() override;

    // Game 진행 도중에 호출되어 저장되었던 상태를 통해 Load 하는 함수입니다.
    // Callback 지원을 위해 DoRecoveryCallbacks 함수를 호출해야합니다.
    virtual void Recovery() override;

private:
    // decreaseFactor는 초당 감쇠율
    coroutine::Coroutine ShakeCoroutine(float strength, float frequency, float decreaseFactor, Vector3d origin);
    void UpdateCameraView();
    factor::Adder<Vector3d> camOffsetAdder;
    std::vector<std::weak_ptr<coroutine::Coroutine>> shakeCoroutines;
    const application::editor::RegionData* contrainingRegion{ nullptr };
    Vector3d euler;
    //GameObject* target = nullptr;
    // 복수의 타겟이 지정되면, 타겟들의 중점을 따라가게 됩니다.
    std::vector<GameObject*> targets;
    Vector3d distance;
    Vector3d idealPosition;
    Quaternion idealRotation;

    Vector3d positionDelta = Vector3d();

    Vector2d mousePos = Vector2d();
    Vector2d beforeMousePos = Vector2d();

    Quaternion rot;
    Quaternion ingameInitRot;
    Vector3d ingameInitScale;

    float initFov;
    float initNear;
    float initFar;
    float initWidth;
    float initHeight;

    float pitch = 0.0;
    float pitchDelta = 0.0;
    float yaw = 0.0;
    float yawDelta = 0.0;

    float min_Speed = 0.002;
    float max_Speed = 2.0;
    float defaultMoveSpeed = 0.03;
    float rotationSpeed = 0.3;

    bool updateability = true;
};
