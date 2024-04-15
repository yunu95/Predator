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
        virtual void SetBone(std::wstring fbxName) = 0;
        virtual void SetAnimatorIndex(int animatorIndex) = 0;
		virtual void SetMaterial(unsigned int index, IMaterial* material, bool isOrigin = false) {};
		virtual IMaterial* GetMaterial(unsigned int index = 0, bool isInstance = true) { return nullptr; };
    };

    struct ISkinnedMeshDesc
    {
    };
}
