#pragma once
#include "YunuGraphicsInterface.h"

namespace yunuGIAdapter
{
	class LightAdapter : virtual public yunuGI::ILight
	{
	public:
		LightAdapter() { }
		virtual ~LightAdapter() = default;
		static LightAdapter* GetWrapper() { }

		virtual void SetWorldTM(const yunuGI::Matrix4x4& worldTM) { };
		virtual void SetActive(bool isActive) { };
	private:
	};
}
