#pragma once
#include "YunutyEngine.h"
#include "FileSystem.h"

namespace wanderUtils
{
    void LoadResourcesRecursively()
    {
        const yunuGI::IResourceManager* resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();

        /// SCRES 우선 로드
        resourceManager->LoadFile("FBXMaterial.scres");

        // 나머지 기타등등 파일들 로드하기
        {
            namespace fs = std::filesystem;
            std::set<std::string> validExtensions{ ".jpg", ".bmp", ".tga", ".dds", ".cso",".png" };
            fs::path basePath{ "./" };
            try
            {
                if (fs::exists(basePath) && fs::is_directory(basePath))
                {
                    for (const auto& entry : fs::recursive_directory_iterator(basePath))
                    {
                        if (fs::is_regular_file(entry) && validExtensions.contains(entry.path().extension().string()))
                        {
                            auto relativePath = fs::relative(entry.path(), basePath).string();
                            std::replace(relativePath.begin(), relativePath.end(), '\\', '/');
                            resourceManager->LoadFile(relativePath.c_str());
                        }
                    }
                }
            }
            catch (const fs::filesystem_error& err) {
                std::cerr << "Error: " << err.what() << std::endl;
            }
        }

        // FBX 로드하기
        {
            auto directorList = application::editor::fileSystem::GetSubdirectories("FBX");
            for (auto each : directorList)
            {
                resourceManager->LoadFile(("FBX/" + each.string()).c_str());
            }
        }
    }
}
