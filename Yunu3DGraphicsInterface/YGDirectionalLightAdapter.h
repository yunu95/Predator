#pragma once
#include <unordered_map>
#include "YunuGraphicsInterface.h"
#include "YGLightAdapter.h"

namespace yunuGIAdapter
{
	class DirectionalLightAdapter :virtual public yunuGIAdapter::LightAdapter, virtual public yunuGI::IDirectionalLight
	{
	public:
		DirectionalLightAdapter() :LightAdapter() { }
		
	private:

	};
}
