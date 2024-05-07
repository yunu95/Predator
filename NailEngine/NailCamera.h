#pragma once

#include <DirectXCollision.h>
#include <DirectXMath.h>
#include "SimpleMath.h"
using namespace DirectX::PackedVector;

class NailCamera
{
public:
    void GetCameraAreaXZ(DirectX::SimpleMath::Vector2& minPoint, DirectX::SimpleMath::Vector2& maxPoint);
    void SetWorldTM(const DirectX::SimpleMath::Matrix wtm);
    void SetVerticalFOV(float fov);
    void SetNear(float cameraNear);
    void SetFar(float cameraFar);
    void SetResolution(float width, float height);
    float GetVerticalFOV();
    float GetNear();
    float GetFar();
    void GetResolution(float* width, float* height);
    void SetAsMain();

    DirectX::SimpleMath::Matrix& GetVTM() { return this->vtm; }
    DirectX::SimpleMath::Matrix& GetVTMOrtho() { return this->vtmOrtho; }
    DirectX::SimpleMath::Matrix& GetPTM() { return this->ptm; }
    DirectX::SimpleMath::Matrix& GetWTM() { return this->wtm; }

    DirectX::SimpleMath::Matrix GetPTM90();
    DirectX::SimpleMath::Vector2 GetScreenPos(const DirectX::SimpleMath::Vector3& worldPos);

        DirectX::BoundingFrustum& GetFrustum()
    {
        return this->frustum;
    };

private:
    void CreateFrustum();

private:
    DirectX::SimpleMath::Matrix wtm;
    DirectX::SimpleMath::Matrix vtm;
    DirectX::SimpleMath::Matrix vtmOrtho;
    DirectX::SimpleMath::Matrix ptm;
    float fov = DirectX::XM_PI / 4.f;
    float cameraNear = 0.1f;
    float cameraFar = 1000.f;
    float width = 1920;
    float height = 1080;

    DirectX::BoundingFrustum frustum;
};

