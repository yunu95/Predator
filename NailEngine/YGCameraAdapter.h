#pragma once
#include "IYunuGICamera.h"
#include "YunuVector2.h"
#include "YunuVector3.h"

#include "CameraManager.h"
#include "NailCamera.h"

#include <DirectXMath.h>
#include "SimpleMath.h"
using namespace DirectX::PackedVector;

namespace yunuGIAdapter
{
    class CameraAdapter : public yunuGI::ICamera
    {
    public:
        CameraAdapter()
        {
            CameraManager::Instance.Get().RegisterCamera(this, std::make_shared<NailCamera>());
        };

        virtual ~CameraAdapter() {};

        virtual void SetWorldTM(const yunuGI::Matrix4x4& tm) override
        {
            CameraManager::Instance.Get().SetWorldTM(this, reinterpret_cast<const DirectX::SimpleMath::Matrix&>(tm));
        };

        virtual void SetVerticalFOV(float fov) override
        {
            CameraManager::Instance.Get().SetVerticalFOV(this, fov);
        };

        virtual float GetVerticalFOV() override
        {
            return CameraManager::Instance.Get().GetVerticalFOV(this);
        }

        virtual void SetNear(float cameraNear) override
        {
            CameraManager::Instance.Get().SetNear(this, cameraNear);
        };

        virtual float GetNear()
        {
            return CameraManager::Instance.Get().GetNear(this);
        }

        virtual void SetFar(float cameraFar) override
        {
            CameraManager::Instance.Get().SetFar(this, cameraFar);
        };

        virtual float GetFar() override
        {
            return CameraManager::Instance.Get().GetFar(this);
        }

        virtual void SetResolution(float width, float height) override
        {
            CameraManager::Instance.Get().SetResolution(this, width, height);
        };

        virtual void GetResolution(float* width, float* height)
        {
            CameraManager::Instance.Get().GetResolution(this, width, height);
        }

        virtual void SetAsMain() override
        {
            CameraManager::Instance.Get().ChangeMainCamera(this);
        };
        virtual yunuGI::Vector2 GetScreenPos(const yunuGI::Vector3& worldPos) override
        {
            auto screenPos = CameraManager::Instance.Get().GetScreenPos(this, reinterpret_cast<const DirectX::SimpleMath::Vector3&>(worldPos));
            yunuGI::Vector2 retVal = reinterpret_cast<const yunuGI::Vector2&>(screenPos);
            return retVal;
        }

    };
}
