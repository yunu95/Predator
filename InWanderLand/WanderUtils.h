#pragma once
//#include "InWanderLand.h"
#include "YunutyEngine.h"
#include "FileSystem.h"
#include "Unit.h"

#include <string>
#include <future>
#include <thread>

using namespace application;
namespace wanderUtils
{
	void ChangeStageToOne()
	{
		graphics::Camera::GetMainCamera()->GetGI().SetClearColor(yunuGI::Color{ 0.7686,0.8784,0.9451,1 });
		yunutyEngine::graphics::Renderer::SingleInstance().SetLightMap(L"Stage1LightMap");
	}

    void ChangeStageToTwo()
    {
        graphics::Camera::GetMainCamera()->GetGI().SetClearColor(yunuGI::Color{0,0,0,1});
        yunutyEngine::graphics::Renderer::SingleInstance().SetLightMap(L"Stage2LightMap");
    }

    struct CompareDistance
    {
        Vector3d from;
        CompareDistance(const Vector3d& from) : from(from) {}

        bool operator()(const std::weak_ptr<Unit>& a, const std::weak_ptr<Unit>& b) const {
            if (auto a_ptr = a.lock()) {
                if (auto b_ptr = b.lock()) {
                    return (from - a_ptr->GetTransform()->GetWorldPosition()).MagnitudeSqr() <
                        (from - b_ptr->GetTransform()->GetWorldPosition()).MagnitudeSqr();
                }
            }
            return false;
        }
    };
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
        static void Load(string rootFolder = "./", std::set<std::string> validExtensions = { ".jpg", ".bmp", ".tga", ".dds", ".cso",".png",".mp4" });
        struct boolGuard
        {
            boolGuard(bool& b) : b(b) {}
            ~boolGuard() { b = !b; }
            bool& b;
        };
        // 이 로드 함수는 파일을 부를떄 중간중간마다 리턴을 해준다.
        static coroutine::Coroutine LoadByCoroutine(string rootFolder = "./");
    private:
#ifdef EDITOR
        static std::unordered_map<string, float> timeTookToLoadResources;
#endif
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

    /// 자유낙하 운동에서 시간이 정해질 때의 초기 속도를 얻습니다.
    /// 절대 좌표계상에서 방향 벡터를 반환하며, 크기가 초기 속력입니다.
    /// GlobalConstant 의 gravitySpeed 를 가속도로 계산합니다.
    Vector3d GetInitSpeedOfFreeFall(float duration, Vector3d startPos, Vector3d destPos);
}
