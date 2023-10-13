#pragma once

#include "IYunuGIRenderable.h"

namespace yunuGI
{
	class IMaterial;

	class IMeshRenderer : public IRenderable
	{
	public:
		virtual void SetMaterial(IMaterial* material) = 0;
		virtual IMaterial* GetMaterial() = 0;

	};
}
