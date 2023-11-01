#pragma once
#include "IYunuMeshRenderer.h"
#include "YunuGIMatrix4x4.h"
#include "IUnknown.h"

#include <string>

namespace yunuGI
{
	class IStaticMeshRenderer : public yunuGI::IMeshRenderer
	{
	public:
		virtual void SetMesh(const std::wstring& meshName) = 0;
		virtual void SetPickingMode(bool isPickingModeOn) = 0;
		virtual void SetMaterial(unsigned int index, IMaterial* material) {};
		virtual IMaterial* GetMaterial(unsigned int index = 0) { return nullptr; };
	};

	struct IStaticMeshRendererDesc
	{
	};
}
