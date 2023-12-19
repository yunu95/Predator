#include "CppUnitTest.h"
#include "EditorLayer.h"
#include "YunutyEngine.h"
#include "DelayedTestFunctions.h"
#include "Application.h"
#include "MapFileManager.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

void TestCaseMapFileIOInitializer()
{
    
}

namespace InWanderLand
{
    TEST_CLASS(InWanderLand)
    {
    public:
        TEST_METHOD(TestCaseMapFileIO)
        {
            Application::Application& client = Application::Application::CreateApplication(0, 0);
            Application::Editor::EditorLayer::AssignTestInitializer(TestCaseMapFileIOInitializer);
            client.Initialize();

            auto& mfm = Application::Editor::MapFileManager::GetInstance();
            
            mfm.SaveMapFile("TestMap");

            mfm.LoadMapFile("TestMap");

            


            Assert::IsTrue(1 + 1 == 2);
        }
    };
}