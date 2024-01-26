#pragma once
#include <vector>
#include <filesystem>
#include "YunuGraphicsInterface.h"

#include "Utils.h"
#include "ResourceManager.h"

namespace yunuGIAdapter
{
    class ResourceManagerAdapter : public yunuGI::IResourceManager
    {
    public:
        virtual void LoadFile(const char* filePath)const override
        {
            std::wstring ext = std::filesystem::path(filePath).extension();
            std::string fileName = std::filesystem::path(filePath).filename().string();
            std::string parentFolderName = std::filesystem::path(filePath).parent_path().filename().string();

            int bufferSize = MultiByteToWideChar(CP_UTF8, 0, filePath, -1, nullptr, 0);
            bufferSize--;
			std::wstring wFilePath(bufferSize, L'\0');
			MultiByteToWideChar(CP_UTF8, 0, filePath, -1, &wFilePath[0], bufferSize);


            std::filesystem::path topLevelPath;
			for (const auto& p : std::filesystem::path(filePath)) {
				topLevelPath = p;
				break;  // 첫 번째 경로 세그먼트만 가져오고 반복문 종료
			}

            if (ext == L".cso")
            {
                ResourceManager::Instance.Get().CreateShader(wFilePath);
            }
            else if (parentFolderName == "FBX")
            {
                parentFolderName = parentFolderName + "/" + fileName + "/" + fileName + ".fbx";
                ResourceManager::Instance.Get().LoadFBX(parentFolderName.c_str());
            }
            else if (topLevelPath.string() == "Texture")
            {
                ResourceManager::Instance.Get().CreateTexture(wFilePath);
            }
        };
        virtual yunuGI::IMaterial* CreateMaterial(std::wstring materialName) const
        {
            return ResourceManager::Instance.Get().CrateMaterial(materialName);
        };
        virtual yunuGI::IMesh* CreateMesh(std::wstring meshName, std::vector<yunuGI::Vector3>& posVec, std::vector<unsigned int>& idxVec, std::vector<yunuGI::Vector3>& normalVec)const override
        {
            return ResourceManager::Instance.Get().CreateMesh(meshName, posVec,idxVec,normalVec);
        };
        virtual yunuGI::IMaterial* GetMaterial(const std::wstring& materialName)const
        {
            return ResourceManager::Instance.Get().GetMaterial(materialName).get();
        };
        virtual yunuGI::IMesh* GetMesh(const std::wstring& meshName)const
        {
            return reinterpret_cast<yunuGI::IMesh*>(ResourceManager::Instance.Get().GetMesh(meshName).get());
        }
        virtual bool GetFBXData(const std::string& fbxName, yunuGI::FBXData*& fbxData)const override
        {
            auto _fbxData = ResourceManager::Instance.Get().GetFBXData(fbxName);
            fbxData = _fbxData;
            if (_fbxData == nullptr)
            {
                return false;
            }
            
            return true;
        };

        virtual bool GetFBXBoneData(const std::string& fbxName, yunuGI::BoneInfo& boneInfo)const override
        {
            auto& fbxBoneData = ResourceManager::Instance.Get().GetBoneData(fbxName);
            
            //boneInfo = std::move(fbxBoneData);
            //if (boneInfo.name.empty())
            //{
            //    return false;
            //}

            return true;
        };

        virtual void UnloadResources()const override
        {
        };
        virtual void* GetFinalRenderImage() const override
        {
            return ResourceManager::Instance.Get().GetFinalRenderImage();
        };
        virtual void* GetDevice() const override
        {
            return ResourceManager::Instance.Get().GetDevice();
        };
        virtual void* GetDeviceContext() const override
        {
            return ResourceManager::Instance.Get().GetDeviceContext();
        };
        virtual std::vector<yunuGI::IShader*>& GetShaderList() const override
        {
            return ResourceManager::Instance.Get().GetShaderList();
        }

		virtual std::vector<yunuGI::IMesh*>& GetMeshList() const override
		{
			return ResourceManager::Instance.Get().GetMeshList();
		}

        virtual std::vector<yunuGI::IMaterial*>& GetMaterialList()const override
		{
			return ResourceManager::Instance.Get().GetMaterialList();
		}

        virtual std::vector<yunuGI::IAnimation*>& GetAnimationList()const override
        {
            return ResourceManager::Instance.Get().GetAnimationList();
        };

        virtual std::vector<yunuGI::ITexture*>& GetTextureList()const override
        {
            return ResourceManager::Instance.Get().GetTextureList();
        };
    };
}
