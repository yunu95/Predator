#pragma once
#include "IYunuGICamera.h"

#include "NailCamera.h"

#include <DirectXMath.h>
#include "SimpleMath.h"
using namespace DirectX::PackedVector;

namespace yunuGIAdapter
{
    class CameraAdapter : public yunuGI::ICamera
    {
    public:
        CameraAdapter() {};
        virtual ~CameraAdapter() {};
        virtual void SetWorldTM(const yunuGI::Matrix4x4& tm) override
        {
            NailCamera::Instance.Get().SetWorldTM(reinterpret_cast<const DirectX::SimpleMath::Matrix&>(tm));
        };

        virtual void SetFOV(float fov) override 
        {
            NailCamera::Instance.Get().SetFOV(fov);
        };

        virtual void SetNear(float cameraNear) override
        {
            NailCamera::Instance.Get().SetNear(cameraNear);
        };

        virtual void SetFar(float cameraFar) override 
        {
            NailCamera::Instance.Get().SetFar(cameraFar);
        };

        virtual void SetResolution(float width, float height) override 
        {
            NailCamera::Instance.Get().SetResolution(width, height);
        };

        virtual void SetAsMain() override 
        {
            NailCamera::Instance.Get().SetAsMain();
        };
		
    };
}
