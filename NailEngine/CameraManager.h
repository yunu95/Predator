#pragma once

#include "IYunuGICamera.h"

#include "Utils.h"

#include <memory>
#include <map>

#include <DirectXMath.h>
#include "SimpleMath.h"
using namespace DirectX::PackedVector;

class NailCamera;
class yunuGI::ICamera;

class CameraManager
{
public:
    static LazyObjects<CameraManager> Instance;
    friend LazyObjects<CameraManager>;

public:
    std::shared_ptr<NailCamera> GetMainCamera() { return mainCamera; }

    void RegisterCamera(yunuGI::ICamera* adapter, std::shared_ptr<NailCamera> camera);

    void ChangeMainCamera(yunuGI::ICamera* adapter);

    std::map<yunuGI::ICamera*, std::shared_ptr<NailCamera>>& GetCamearaList();

public:
    void SetWorldTM(yunuGI::ICamera* adapter, DirectX::SimpleMath::Matrix wtm);
    void SetVerticalFOV(yunuGI::ICamera* adapter, float fov);
    float GetVerticalFOV(yunuGI::ICamera* adapter);
    void SetNear(yunuGI::ICamera* adapter, float cameraNear);
    float GetNear(yunuGI::ICamera* adapter);
    void SetFar(yunuGI::ICamera* adapter, float cameraFar);
    float GetFar(yunuGI::ICamera* adapter);
    void SetResolution(yunuGI::ICamera* adapter, float width, float height);
    void GetResolution(yunuGI::ICamera* adapter, float* width, float* height);
    DirectX::SimpleMath::Vector2 GetScreenPos(yunuGI::ICamera* adapter, const DirectX::SimpleMath::Vector3& worldPos);

    DirectX::SimpleMath::Matrix GetPTM90ByResolution(float width, float height, float farPlane, float nearPlane);


private:
    std::map<yunuGI::ICamera*, std::shared_ptr<NailCamera>> cameraMap;
    std::shared_ptr<NailCamera> mainCamera;

};

