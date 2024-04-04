#pragma once
#include "IYunuMeshRenderer.h"
#include "YunuGIMatrix4x4.h"
#include "IUnknown.h"

#include <string>

namespace yunuGI
{
	class IParticleRenderer : public yunuGI::IMeshRenderer
	{
	public:
		virtual void SetPickingMode(bool isPickingModeOn) = 0;
		virtual void SetMaterial(unsigned int index, IMaterial* material) {};
		virtual IMaterial* GetMaterial(unsigned int index = 0) { return nullptr; };


		virtual void SetDuration(float duration) = 0;
		virtual float GetDuration() = 0;
	};

	struct IParticleRendererDesc
	{
	};
}
