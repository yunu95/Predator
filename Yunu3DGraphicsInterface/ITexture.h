#pragma once
#include "IResource.h"

namespace yunuGI
{
	class ITexture : virtual public IResource
	{
	public:
		virtual float GetWidth() = 0;
		virtual float GetHeight() = 0;
	};
}