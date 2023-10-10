#pragma once
#include "IYunuGIRenderable.h"
#include "YunuGIMatrix4x4.h"
#include "IUnknown.h"

namespace yunuGI
{
	class IStaticMeshRenderer : virtual public yunuGI::IRenderable
	{
	public:
		virtual void LoadMesh(const char* fileName) = 0;
		virtual void SetPickingMode(bool isPickingModeOn) = 0;

	};

	struct IStaticMeshRendererDesc
	{
	};
}
