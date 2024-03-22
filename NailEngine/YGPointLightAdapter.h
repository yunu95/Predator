#pragma once
#include <unordered_map>
#include "YunuGraphicsInterface.h"
#include "YGLightAdapter.h"

#include "LightManager.h"
#include "PointLight.h"

namespace yunuGIAdapter
{
	class PointLightAdapter :virtual public yunuGIAdapter::LightAdapter, virtual public yunuGI::IPointLight
	{
	public:
		PointLightAdapter() :LightAdapter() 
		{
			this->light = std::make_shared<PointLight>();

			LightManager::Instance.Get().PushLightInstance(this->light);
		}

		virtual ~PointLightAdapter()
		{
			LightManager::Instance.Get().PopLightInstance(this->light);
		}

		virtual void SetLightDiffuseColor(yunuGI::Color& color) override
		{
			light->SetLightDiffuseColor(reinterpret_cast<DirectX::SimpleMath::Vector4&>(color));
		};

		virtual void SetWorldTM(const yunuGI::Matrix4x4& tm) override
		{
			DirectX::SimpleMath::Matrix wtm = reinterpret_cast<const DirectX::SimpleMath::Matrix&>(tm);
			DirectX::SimpleMath::Vector3 pos;
			DirectX::SimpleMath::Vector3 scale;
			DirectX::SimpleMath::Quaternion quat;
			wtm.Decompose(scale, quat, pos);

			DirectX::SimpleMath::Matrix rot = DirectX::XMMatrixRotationQuaternion(quat);

			// 라이트의 위치를 넣어줌
			DirectX::SimpleMath::Vector4 lightPos = { pos.x,pos.y,pos.z, 1.f };
			light->SetLightPosition(lightPos);

			light->SetWorldTM(reinterpret_cast<const DirectX::SimpleMath::Matrix&>(tm));
		}

		virtual void SetRange(float range) override
		{
			light->SetRange(range);
		};

	private:
		std::shared_ptr<PointLight> light;
	};
}