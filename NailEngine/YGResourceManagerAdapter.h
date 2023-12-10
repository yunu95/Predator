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
			std::wstring wFilePath(bufferSize, 0);
			MultiByteToWideChar(CP_UTF8, 0, filePath, -1, &wFilePath[0], bufferSize);

            if (ext == L".hlsl")
            {
                ResourceManager::Instance.Get().CreateShader(wFilePath);
            }
            else if (parentFolderName == "FBX")
            {
                parentFolderName = parentFolderName + "/" + fileName + "/" + fileName + ".fbx";
                ResourceManager::Instance.Get().LoadFBX(parentFolderName.c_str());
            }
        };
        virtual yunuGI::IMaterial* CreateMaterial(std::wstring materialName) const
        {
            return ResourceManager::Instance.Get().CrateMaterial(materialName);
        };
        virtual yunuGI::IMaterial* GetMaterial(const std::wstring& materialName)const
        {
            return ResourceManager::Instance.Get().GetMaterial(materialName).get();
        };
        virtual yunuGI::IMesh* GetMesh(const std::wstring& meshName)const
        {
            return reinterpret_cast<yunuGI::IMesh*>(ResourceManager::Instance.Get().GetMesh(meshName).get());
        }
        virtual bool GetFBXData(const std::string& fbxName, std::vector<yunuGI::FBXData>& dataVec)const override
        {
            auto& fbxDataVec = ResourceManager::Instance.Get().GetFBXData(fbxName);
            dataVec = std::move(fbxDataVec);
            if (dataVec.size() == 0)
            {
                return false;
            }
            
            return true;
        };

        virtual bool GetFBXBoneData(const std::string& fbxName, yunuGI::BoneInfo& boneInfo)const override
        {
            auto& fbxBoneData = ResourceManager::Instance.Get().GetBoneData(fbxName);
            
            boneInfo = std::move(fbxBoneData);
            if (boneInfo.name.empty())
            {
                return false;
            }

            return true;
        };

        virtual void UnloadResources()const override
        {
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
    };
}
