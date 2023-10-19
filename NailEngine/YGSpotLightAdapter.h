#pragma once
#include <unordered_map>
#include <memory>
#include "YunuGraphicsInterface.h"
#include "YGLightAdapter.h"

#include "LightManager.h"
#include "SpotLight.h"

namespace yunuGIAdapter
{
	class SpotLightAdapter :virtual public yunuGIAdapter::LightAdapter, virtual public yunuGI::ISpotLight
	{
	public:
		SpotLightAdapter() :LightAdapter()
		{
			this->light = std::make_shared<SpotLight>();

			LightManager::Instance.Get().PushLightInstance(this->light);
		}

		virtual void SetWorldTM(const yunuGI::Matrix4x4& tm) override
		{
			DirectX::SimpleMath::Matrix wtm = reinterpret_cast<const DirectX::SimpleMath::Matrix&>(tm);
			DirectX::SimpleMath::Vector3 pos;
			DirectX::SimpleMath::Vector3 scale;
			DirectX::SimpleMath::Quaternion quat;
			wtm.Decompose(scale, quat, pos);

			DirectX::SimpleMath::Matrix rot = DirectX::XMMatrixRotationQuaternion(quat);

			// 라이트의 위치를 넣어줌
			DirectX::SimpleMath::Vector4 lightPos = { pos.x,pos.y,pos.z,1.f };
			light->SetLightPosition(lightPos);
			DirectX::SimpleMath::Vector4 front{ 0.f,0.f,1.f,0.f };
			front = DirectX::XMVector3Rotate(front, quat);
			light->SetLightDirection(front);
		}

		virtual void SetLightDiffuseColor(yunuGI::Color& color) override
		{
			light->SetLightDiffuseColor(reinterpret_cast<DirectX::SimpleMath::Vector4&>(color));
		}

		virtual void SetRange(float range) override
		{
			light->SetRange(range);
		}

		virtual void SetAngle(float angle) override
		{
			light->SetAngle(angle);
		}

	private:
		std::shared_ptr<SpotLight> light;
	};
}
