#pragma once
#include <vector>
#include <string>
#include "IUnknown.h"

#include "IMaterial.h"

// 리소스 매니저는 텍스처, 메시와 같은 그래픽스 리소스들을 명시적으로 불러오거나 할당해제하고 싶을 때 사용한다.
namespace yunuGI
{
    struct IResourceManagerDesc
    {

    };
    class IResourceManager : public IUnknown
    {
    public:
        virtual std::vector<std::string> GetMeshKeys() const = 0;
        virtual std::vector<std::string> GetSkeletonKeys()const = 0;
        virtual std::vector<std::string> GetTextureKeys()const = 0;
        virtual std::vector<std::string> AnimationKeys()const = 0;
        virtual void LoadFile(const char* filePath)const = 0;
        virtual yunuGI::IMaterial* CreateMaterial(yunuGI::MaterialDesc& materialDesc)const = 0;
        virtual void UnloadResources()const = 0;
    };
}

