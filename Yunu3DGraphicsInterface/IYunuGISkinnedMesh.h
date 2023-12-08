#pragma once
#include "IYunuGIRenderable.h"
#include "YunuGIMatrix4x4.h"
#include "IUnknown.h"

namespace yunuGI
{
    class ISkinnedMesh : public yunuGI::IMeshRenderer
    {
    public:
		virtual void SetPickingMode(bool isPickingModeOn) = 0;
		virtual void SetMaterial(unsigned int index, IMaterial* material) {};
		virtual IMaterial* GetMaterial(unsigned int index = 0) { return nullptr; };
    };

    struct ISkinnedMeshDesc
    {
    };
}
