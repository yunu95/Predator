#pragma once
#include "IYunuMeshRenderer.h"
#include "YunuGIMatrix4x4.h"
#include "IUnknown.h"
#include "ParticleRenderInfo.h"

#include <list>

namespace yunuGI
{
	class ITexture;
	class IParticleRenderer : public yunuGI::IMeshRenderer
	{
	public:
		virtual void SetPickingMode(bool isPickingModeOn) = 0;
		virtual void SetMaterial(unsigned int index, IMaterial* material, bool isOrigin = false) {};
		virtual void SetTexture(yunuGI::ITexture* texture) = 0;

		virtual void SetDuration(float duration) = 0;
		virtual float GetDuration() = 0;
		virtual void SetParticleInfoList(std::list<yunuGI::ParticleRenderInfo>& particleRenderInfo) = 0;
	};

	struct IParticleRendererDesc
	{
	};
}
