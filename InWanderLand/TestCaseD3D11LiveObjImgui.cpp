#include "CppUnitTest.h"
#include "ContentsLayer.h"
#include "YunutyEngine.h"
#include "DelayedTestFunctions.h"
#include "Application.h"
#include "RTSCam.h"
#include "YunutyEngine.h"
#include "DelayedTestFunctions.h"
#include "ShakyCam.h"
#include "DebugTilePlane.h"
#include "DebugBeacon.h"
#include "DebugMeshes.h"
#include "YunutyEngine.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// 이 함수는 게임의 기본 초기화 함수를 오버라이드합니다.
void TestCaseD3D11LiveObjimguiInit()
{
}

namespace InWanderLand
{
    TEST_CLASS(InWanderLand)
    {
    public:
        TEST_METHOD(TestCaseD3D11LiveObjimgui)
        {
            try
            {
                Application::Application& client = Application::Application::CreateApplication(0, 0);
                Application::Contents::ContentsLayer::AssignTestInitializer(TestCaseD3D11LiveObjimguiInit);
                client.Initialize();
                client.Run();
                client.Finalize();
            }
            catch (const std::exception& e)
            {
                std::cout << e.what();
                Assert::Fail(L"Exception was thrown");
            }
            //Assert::IsTrue();
        }
    };
}