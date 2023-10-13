#pragma once
#include "IYunuMeshRenderer.h"
#include "YunuGIMatrix4x4.h"
#include "IUnknown.h"

namespace yunuGI
{
	class IStaticMeshRenderer : public yunuGI::IMeshRenderer
	{
	public:
		virtual void LoadMesh(const char* fileName) = 0;
		virtual void SetPickingMode(bool isPickingModeOn) = 0;
		virtual void SetMaterial(IMaterial* material) {};
		virtual IMaterial* GetMaterial() { return nullptr; };
	};

	struct IStaticMeshRendererDesc
	{
	};
}
