#pragma once
#include "YunutyEngine.h"
#include "DebugBeacon.h"
#include "Unit.h"
#include "UIManager.h"
#include "ContentsObservee.h"

using namespace yunutyEngine;

namespace application
{
    namespace editor
    {
        class RegionData;
    }
}

class RTSCam : public yunutyEngine::graphics::Camera, public ContentsObservee
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
    // 카메라가 지역 밖으로 나가지 못하게 가둠.
    void UnConstrainRegion();
    // 카메라가 지역 밖으로 나가지 못하게 가둠.
    void ConstrainByRegion(const application::editor::RegionData* region);
    void SetTarget(GameObject* target);
    void SetTargets(const std::vector<GameObject*>& targets);
    float GetCameraSpeed() const;

    void SetUpdateability(bool updateability) { this->updateability = updateability; }
    bool GetUpdateability() { return updateability; }
    Vector3d GetIdealPosition();
    Quaternion GetInitRotation() { return ingameInitRot; }
    Vector3d GetInitScale() { return ingameInitScale; }
    float GetInitVerticalFOV() { return initFov; }
    float GetInitNear() { return initNear; }
    float GetInitFar() { return initFar; }
    float GetInitWidth() { return initWidth; }
    float GetInitHeight() { return initHeight; }

    virtual void PlayFunction() override;
    virtual void StopFunction() override;

private:
    void UpdateCameraView();
    const application::editor::RegionData* contrainingRegion{ nullptr };
    Vector3d euler;
    //GameObject* target = nullptr;
    // 복수의 타겟이 지정되면, 타겟들의 중점을 따라가게 됩니다.
    std::vector<GameObject*> targets;
    Vector3d distance;

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
