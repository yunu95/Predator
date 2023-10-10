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

        virtual std::vector<std::string> GetMeshKeys() const override { return std::vector<std::string>{}; }
        virtual std::vector<std::string> GetSkeletonKeys() const override { return std::vector<std::string>{}; }
        virtual std::vector<std::string> GetTextureKeys() const override { return std::vector<std::string>{}; }
        virtual std::vector<std::string> AnimationKeys() const override { return std::vector<std::string>{}; }
        virtual void LoadFile(const char* filePath)const override
        {
            std::wstring ext = std::filesystem::path(filePath).extension();

			int bufferSize = MultiByteToWideChar(CP_UTF8, 0, filePath, -1, nullptr, 0);
			std::wstring wFilePath(bufferSize, 0);
			MultiByteToWideChar(CP_UTF8, 0, filePath, -1, &wFilePath[0], bufferSize);

            if (ext == L"hlsl")
            {
                ResourceManager::Instance.Get().CreateShader(wFilePath);
            }
        };
        virtual yunuGI::IMaterial* CreateMaterial(yunuGI::MaterialDesc& materialDesc) const
        {
            return ResourceManager::Instance.Get().CrateMaterial(materialDesc);
        };

        virtual void UnloadResources()const override
        {
        };
    };
}
