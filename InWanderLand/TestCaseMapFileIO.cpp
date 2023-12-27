#ifdef GEN_TESTS
#include "CppUnitTest.h"
#include "EditorLayer.h"
#include "YunutyEngine.h"
#include "DelayedTestFunctions.h"
#include "Application.h"
#include "MapFileManager.h"
#include "TemplateDataManager.h"
#include "InstanceManager.h"
#include "Storable.h"

#include <fstream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

void TestCaseMapFileIOInitializer()
{
    using namespace application::editor;
    auto& tdm = TemplateDataManager::GetSingletonInstance();
    tdm.CreateTemplateData("Terrain1", DataType::TerrainData);
    tdm.CreateTemplateData("Terrain2", DataType::TerrainData);
    tdm.CreateTemplateData("Terrain3", DataType::TerrainData);
    tdm.CreateTemplateData("Unit1", DataType::UnitData);
    tdm.CreateTemplateData("Unit2", DataType::UnitData);
    tdm.CreateTemplateData("Unit3", DataType::UnitData);
    tdm.CreateTemplateData("Ornament1", DataType::OrnamentData);
    tdm.CreateTemplateData("Ornament2", DataType::OrnamentData);
    tdm.CreateTemplateData("Ornament3", DataType::OrnamentData);

    auto& im = InstanceManager::GetSingletonInstance();
    im.CreateInstance("Terrain1");
    im.CreateInstance("Terrain2");
    im.CreateInstance("Terrain2");
    im.CreateInstance("Terrain3");
    im.CreateInstance("Terrain3");
    im.CreateInstance("Terrain3");
    im.CreateInstance("Unit1");
    im.CreateInstance("Unit2");
    im.CreateInstance("Unit2");
    im.CreateInstance("Unit3");
    im.CreateInstance("Unit3");
    im.CreateInstance("Unit3");
    im.CreateInstance("Ornament1");
    im.CreateInstance("Ornament2");
    im.CreateInstance("Ornament2");
    im.CreateInstance("Ornament3");
    im.CreateInstance("Ornament3");
    im.CreateInstance("Ornament3");
}

namespace InWanderLand
{
    TEST_CLASS(InWanderLand)
    {
    public:
        TEST_METHOD(TestCaseMapFileIO)
        {
            application::Application& client = application::Application::CreateApplication(0, 0);
            application::editor::EditorLayer::AssignTestInitializer(TestCaseMapFileIOInitializer);
            client.Initialize();

            auto& mfm = application::editor::MapFileManager::GetSingletonInstance();
            auto& tdm = application::editor::TemplateDataManager::GetSingletonInstance();
            auto& im = application::editor::InstanceManager::GetSingletonInstance();

            mfm.SaveMapFile("TestCase1.pmap");
            mfm.Clear();
            mfm.LoadMapFile("TestCase1.pmap");
            mfm.SaveMapFile("TestCase2.pmap");
            
            json mapData1;
            json mapData2;

            std::ifstream file1{ "TestCase1.pmap" };
            std::ifstream file2{ "TestCase2.pmap" };

            if (file1.is_open())
            {
                file1 >> mapData1;
            }
            else
            {
                Assert::Fail(L"File Open Fail");
            }

            if (file2.is_open())
            {
                file2 >> mapData2;
            }
            else
            {
                Assert::Fail(L"File Open Fail");
            }

            Assert::IsTrue(mapData1 == mapData2, L"The saved and loaded files are different.");
        }
    };
}
#endif