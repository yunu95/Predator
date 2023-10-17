#pragma once
#include <unordered_map>
#include "YunuGraphicsInterface.h"
#include "YGLightAdapter.h"

namespace yunuGIAdapter
{
	class PointLightAdapter :virtual public yunuGIAdapter::LightAdapter, virtual public yunuGI::IPointLight
	{
	public:
		PointLightAdapter() :LightAdapter() { }
		virtual void SetLightDiffuseColor(yunuGI::Color& color) override {};
	private:

	};
}