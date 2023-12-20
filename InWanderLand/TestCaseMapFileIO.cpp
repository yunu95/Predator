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
    auto& tdm = TemplateDataManager::GetInstance();
    tdm.CreateTemplateData("Terrain1", IEditableData::DataType::Terrain);
    tdm.CreateTemplateData("Terrain2", IEditableData::DataType::Terrain);
    tdm.CreateTemplateData("Terrain3", IEditableData::DataType::Terrain);
    tdm.CreateTemplateData("Unit1", IEditableData::DataType::Units);
    tdm.CreateTemplateData("Unit2", IEditableData::DataType::Units);
    tdm.CreateTemplateData("Unit3", IEditableData::DataType::Units);
    tdm.CreateTemplateData("Ornament1", IEditableData::DataType::Ornaments);
    tdm.CreateTemplateData("Ornament2", IEditableData::DataType::Ornaments);
    tdm.CreateTemplateData("Ornament3", IEditableData::DataType::Ornaments);

    auto& im = InstanceManager::GetInstance();
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

            auto& mfm = application::editor::MapFileManager::GetInstance();
            auto& tdm = application::editor::TemplateDataManager::GetInstance();
            auto& im = application::editor::InstanceManager::GetInstance();

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