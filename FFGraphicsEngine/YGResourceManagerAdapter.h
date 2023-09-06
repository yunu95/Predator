#pragma once
#include <vector>
#include "YunuGraphicsInterface.h"

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
        virtual void LoadFile(const char* filePath) override
        {
			// 파일 확장자 얻기
			wstring ext = fs::path(filePath).extension();

            std::wstring _filePath = std::wstring{ filePath, filePath + strlen(filePath) };

            if (ext == L".fbx")
            {
                ResourceManager::GetInstance()->LoadFBX(_filePath);
            }
        };
        virtual void UnloadResources() override
        {
        };
    };
}
