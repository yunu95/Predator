#pragma once
#include "YunutyEngine.h"
#include "FileSystem.h"

#include <string>

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

	/// string 주소에 직접 접근하여 수정한 경우, Size 를 갱신하는 함수입니다.
	void UpdateStringSize(std::string& str)
	{
		int strSize = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0) - 1;
		str.resize(strSize);
	}

	/// wtring 주소에 직접 접근하여 수정한 경우, Size 를 갱신하는 함수입니다.
	void UpdateWStringSize(std::wstring& wstr)
	{
		int wstrSize = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr) - 1;
		wstr.resize(wstrSize);
	}
}
