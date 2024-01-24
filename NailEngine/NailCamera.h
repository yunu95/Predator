#pragma once

#include "Utils.h"

#include <DirectXMath.h>
#include "SimpleMath.h"
using namespace DirectX::PackedVector;

class NailCamera
{
public:
    static LazyObjects<NailCamera> Instance;
    friend LazyObjects<NailCamera>;
	
public:
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

};

