#pragma once
#include "InWanderLand.h"
#include "YunutyEngine.h"
#include "FileSystem.h"

#include <string>
#include <future>
#include <thread>

using namespace application;
namespace wanderUtils
{
    class ResourceRecursiveLoader
    {
    public:
        static float coroutineDeltaTimeThreshold;
        static bool IsLoadingResources();
        static bool IsTimeToReturn(std::chrono::steady_clock::time_point& base)
        {
            if (auto now = std::chrono::high_resolution_clock::now(); std::chrono::duration_cast<std::chrono::milliseconds>(now - base).count() > 1000 * coroutineDeltaTimeThreshold)
            {
                base = now;
                return true;
            }
            return false;
        }
        // 사실상 얘는 로딩화면을 띄우기 위한 최소한의 리소스를 로드하는 데에만 쓰인다.
        static void Load(string rootFolder = "./", std::set<std::string> validExtensions = { ".jpg", ".bmp", ".tga", ".dds", ".cso",".png",".mp4" })
        {
            const yunuGI::IResourceManager* resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();

            /// SCRES 우선 로드
            //resourceManager->LoadFile("FBXMaterial.scres");

            // 나머지 기타등등 파일들 로드하기
            {
                namespace fs = std::filesystem;
                std::set<std::string> soundExtensions{ ".wav", ".mp3", ".wma", ".ogg" };
                fs::path basePath{ rootFolder };
                fs::path realRootPath{ "./" };
                try
                {
                    if (fs::exists(basePath) && fs::is_directory(basePath))
                    {
                        for (const auto& entry : fs::recursive_directory_iterator(basePath))
                        {
#ifdef EDITOR
                            static vector<string> damnableTextures;
                            if (entry.path().extension().string() == ".png" || entry.path().extension().string() == ".jpg" || entry.path().extension().string() == ".bmp" || entry.path().extension().string() == ".tga")
                            {
                                damnableTextures.push_back(entry.path().string());
                            }
#endif
                            if (fs::is_regular_file(entry) && validExtensions.contains(entry.path().extension().string()))
                            {
                                auto relativePath = fs::relative(entry.path(), realRootPath).string();
                                std::replace(relativePath.begin(), relativePath.end(), '\\', '/');
                                resourceManager->LoadFile(relativePath.c_str());
                            }
                            else if (fs::is_regular_file(entry) && soundExtensions.contains(entry.path().extension().string()))
                            {
                                auto relativePath = fs::relative(entry.path(), realRootPath).string();
                                std::replace(relativePath.begin(), relativePath.end(), '\\', '/');
                                yunutyEngine::SoundSystem::LoadSound(relativePath);
                            }
                        }
                    }
                }
                catch (const fs::filesystem_error& err) {
                    std::cerr << "Error: " << err.what() << std::endl;
                }
            }

            // FBX 로드하기
            //{
            //    auto directorList = application::editor::fileSystem::GetSubdirectories(rootFolder + "FBX");
            //    for (auto each : directorList)
            //    {
            //        resourceManager->LoadFile((rootFolder + "FBX/" + each.string()).c_str());
            //    }
            //}
        }
        // 이 로드 함수는 파일을 부를떄 중간중간마다 리턴을 해준다.
        static coroutine::Coroutine LoadByCoroutine(string rootFolder = "./")
        {
            isLoadingResources = true;
            const yunuGI::IResourceManager* resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
            auto enterTime = std::chrono::high_resolution_clock::now();
#ifdef EDITOR
            auto startTime = std::chrono::high_resolution_clock::now();
            std::unordered_map<string, float> timeTookToLoadResources;
#endif
            /// SCRES 우선 로드

            // 기타등등 파일들 로드하기, 이거 잘못하다가 충돌날 수도 있긴 함.
            std::future<void> futureEtc = std::async(std::launch::async, [&]()
                {
                    namespace fs = std::filesystem;
                    resourceManager->LoadFile("FBXMaterial.scres");
                    std::set<std::string> validExtensions{ ".jpg", ".bmp", ".tga", ".dds", ".cso",".png" ,".mp4" };
                    std::set<std::string> soundExtensions{ ".wav", ".mp3", ".wma", ".ogg" };
                    fs::path basePath{ rootFolder };
                    try
                    {
                        if (fs::exists(basePath) && fs::is_directory(basePath))
                        {
                            for (const auto& entry : fs::recursive_directory_iterator(basePath))
                            {
#ifdef EDITOR
                                startTime = std::chrono::high_resolution_clock::now();
#endif
                                if (fs::is_regular_file(entry) && validExtensions.contains(entry.path().extension().string()))
                                {
                                    auto relativePath = fs::relative(entry.path(), basePath).string();
                                    std::replace(relativePath.begin(), relativePath.end(), '\\', '/');
                                    resourceManager->LoadFile(relativePath.c_str());
                                }
                                else if (fs::is_regular_file(entry) && soundExtensions.contains(entry.path().extension().string()))
                                {
                                    auto relativePath = fs::relative(entry.path(), basePath).string();
                                    std::replace(relativePath.begin(), relativePath.end(), '\\', '/');
                                    yunutyEngine::SoundSystem::LoadSound(relativePath);
                                }
#ifdef EDITOR
                                timeTookToLoadResources[entry.path().extension().string()] += std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime).count() / 1000.0f;
#endif
                            }
                        }
                    }
                    catch (const fs::filesystem_error& err) {
                        std::cerr << "Error: " << err.what() << std::endl;
                    }
                });
            while (!futureEtc._Is_ready())
            {
                co_await std::suspend_always{};
            }

            // FBX 로드하기
            {
                auto directorList = application::editor::fileSystem::GetSubdirectories("FBX");
#ifdef EDITOR
#endif
                // FBX 로드는 스레드로 처리해도 오류발생의 위험은 크게 없는 것 같다.
                std::future<void> futureFBX = std::async(std::launch::async, [&]()
                    {
                        for (auto each : directorList)
                        {
                            resourceManager->LoadFile(("FBX/" + each.string()).c_str());
                        }
                    });
                while (!futureFBX._Is_ready())
                {
                    co_await std::suspend_always{};
                }
                // 혹여나 FBX를 스레드를 통해 로드하는 게 자꾸 문제를 일으킨다면 아래의 코루틴 코드를 쓰자.
                //for (auto each : directorList)
                //{
                //    startTime = std::chrono::high_resolution_clock::now();
                //    resourceManager->LoadFile(("FBX/" + each.string()).c_str());
                //    while (IsTimeToReturn(startTime))
                //    {
                //        co_await std::suspend_always{};
                //    }
                //}
#ifdef EDITOR
                timeTookToLoadResources["fbx"] += std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime).count() / 1000.0f;
#endif
            }

            editor::TerrainData* terrainData = nullptr;
            for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::TerrainData>())
            {
                terrainData = each;
            }

            if (terrainData != nullptr)
            {
                std::future<void> navBuildFuture = std::async(std::launch::async, [=]()
                    {
                        terrainData->ApplyAsPlaytimeObject();
                    });
                while (!navBuildFuture._Is_ready())
                {
                    co_await std::suspend_always{};
                }
            }

            isLoadingResources = false;
        }
    private:
        static bool isLoadingResources;
    };


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
