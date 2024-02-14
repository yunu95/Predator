#pragma once
#include <vector>
#include <string>
#include "IUnknown.h"

#include "IMaterial.h"

#include "FBXData.h"

// 리소스 매니저는 텍스처, 메시와 같은 그래픽스 리소스들을 명시적으로 불러오거나 할당해제하고 싶을 때 사용한다.
namespace yunuGI
{
    struct IResourceManagerDesc
    {

    };
    class IResourceManager : public IUnknown
    {
    public:
        virtual void LoadFile(const char* filePath)const = 0;
        virtual yunuGI::IMaterial* CreateMaterial(std::wstring materialName)const = 0;
        virtual yunuGI::IMaterial* CloneMaterial(std::wstring materialName, yunuGI::IMaterial* material)const = 0;
        virtual yunuGI::IMesh* CreateMesh(std::wstring meshName, std::vector<yunuGI::Vector3>& posVec, std::vector<unsigned int>& idxVec, std::vector<yunuGI::Vector3>& normalVec)const = 0;
        virtual void DeleteMesh(yunuGI::IMesh* mesh)const = 0;
        virtual yunuGI::IMaterial* GetMaterial(const std::wstring& materialName)const = 0;
        virtual yunuGI::IMesh* GetMesh(const std::wstring& meshName)const = 0;
        virtual bool GetFBXData(const std::string& fbxName, yunuGI::FBXData*& fbxData)const = 0;
        virtual bool GetFBXBoneData(const std::string& fbxName, yunuGI::BoneInfo& boneInfo)const = 0;
        virtual void UnloadResources()const = 0;

        virtual void* GetFinalRenderImage() const = 0;
        virtual void* GetDevice() const = 0;
        virtual void* GetDeviceContext() const = 0;

        virtual std::vector<IShader*>& GetShaderList()const = 0;
        virtual std::vector<ITexture*>& GetTextureList()const = 0;
        virtual std::vector<IMesh*>& GetMeshList()const = 0;
        virtual std::vector<IMaterial*>& GetMaterialList()const = 0;
        virtual std::vector<IAnimation*>& GetAnimationList()const = 0;
        virtual std::vector<std::wstring>& GetFBXList()const = 0;
    };
}

