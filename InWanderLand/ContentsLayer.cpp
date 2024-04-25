#include "InWanderLand.h"
#include "ContentsLayer.h"

#include "YunutyEngine.h"
#include "ShakyCam.h"
#include "RTSCam.h"   
#include "DebugBeacon.h"
#include "DebugMeshes.h"
#include "Application.h"
#include "TestComponent2.h"
#include "TestUtilGraphicsTestCam.h"
#include "MapFileManager.h"
#include "InstanceManager.h"
#include "SingleNavigationField.h"
#include "TestUtilRTSTestCam.h"
#include "WarriorProductor.h"
#include "MagicianProductor.h"
#include "HealerProductor.h"
#include "BossProductor.h"
#include "EnemySummonGateProductor.h"
#include "InputManager.h"
#include "UIManager.h"
#include "PlayerController.h"
#include "TacticModeSystem.h"
#include "GameManager.h"
#include "SingletonInstanceContainer.h"
#include "ShortcutSystem.h"
#include "RobinSkillDevelopmentSystem.h"
#include "ScriptSystem.h"
#include "Script.h"
#include "UIImage.h"
#include "PlayableComponent.h"
#include "CinematicManager.h"
#include "TutorialManager.h"

#include <algorithm>
#include <string>
#include <filesystem>
#include "WanderUtils.h"

#ifdef GEN_TESTS
#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
#endif

#include <d3d11.h>

std::function<void()> application::contents::ContentsLayer::testInitializer;

bool contentsInputControl = true;

/// 그래픽스 테스트용
void GraphicsTest()
{
	auto camObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	camObj->AddComponent<tests::GraphicsTestCam>();

	const yunuGI::IResourceManager* _resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();

	auto& animationList = _resourceManager->GetAnimationList();
	yunuGI::IAnimation* animation;

	for (auto& i : animationList)
	{
		if (i->GetName() == L"Ani_Monster1_Skill")
		{
			i->SetLoop(true);
			animation = i;
		}
	}

}

void application::contents::ContentsLayer::SetInputControl(bool control)
{
	contentsInputControl = control;
}

bool application::contents::ContentsLayer::GetInputControl()
{
	return contentsInputControl;
}

//void CreateNavPlane(Vector3f botleft, Vector3f topright, std::vector<Vector3f>& worldVertices, std::vector<int>& worldFaces)
//{
//    int startingIdx = worldVertices.size();
//    worldVertices.push_back({ botleft.x,0,topright.z });
//    worldVertices.push_back({ botleft.x,0,botleft.z });
//    worldVertices.push_back({ topright.x,0,botleft.z });
//    worldVertices.push_back({ topright.x,0,topright.z });
//
//    worldFaces.push_back(startingIdx + 2);
//    worldFaces.push_back(startingIdx + 1);
//    worldFaces.push_back(startingIdx + 0);
//    worldFaces.push_back(startingIdx + 3);
//    worldFaces.push_back(startingIdx + 2);
//    worldFaces.push_back(startingIdx + 0);
//
//    auto tilePlane = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<DebugTilePlane>();
//    auto size = topright - botleft;
//    tilePlane->GetTransform()->SetWorldPosition((botleft + topright) / 2.0);
//    tilePlane->width = size.x;
//    tilePlane->height = size.z;
//    tilePlane->SetTiles();
//}
//
//NavigationAgent* CreateAgent(NavigationField* navField)
//{
//    auto agent = yunutyEngine::Scene::getCurrentScene()->AddGameObject()->AddComponent<yunutyEngine::NavigationAgent>();
//    agent->SetSpeed(2);
//    agent->SetRadius(0.5);
//    agent->AssignToNavigationField(navField);
//    auto staticMesh = agent->GetGameObject()->AddGameObject()->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
//
//    const yunuGI::IResourceManager* _resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
//    auto& meshList = _resourceManager->GetMeshList();
//    for (auto& e : meshList)
//    {
//        if (e->GetName() == L"Capsule")
//        {
//            staticMesh->GetGI().SetMesh(e);
//        }
//    }
//
//    staticMesh->GetGI().GetMaterial()->SetColor({ 0.75,0.75,0.75,0 });
//    staticMesh->GetTransform()->position = Vector3d{ 0,0.5,0 };
//    return agent;
//}
void application::contents::ContentsLayer::Initialize()
{
	if (ContentsLayer::testInitializer)
	{
		ContentsLayer::testInitializer();
		return;
	}
	yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());
	ShortcutInit();
	ScriptSystem::Instance();
    CinematicManager::Instance();
	TutorialManager::Instance();

	wanderUtils::LoadResourcesRecursively();

#ifndef EDITOR
#ifdef GRAPHICS_TEST
	{
		yunutyEngine::Collider2D::SetIsOnXYPlane(false);
		auto directionalLight = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		directionalLight->GetTransform()->SetLocalRotation(Quaternion{ Vector3d{90,0,0} });
		directionalLight->GetTransform()->SetLocalPosition(Vector3d{ 0,0,-20 });
		auto light = directionalLight->AddComponent<yunutyEngine::graphics::DirectionalLight>();
		auto color = yunuGI::Color{ 1,1,1,1.f };
		light->GetGI().SetLightDiffuseColor(color);

		//editor::MapFileManager::GetSingletonInstance().LoadStaticOrnaments("TestOrnaments.punreal");
	}
	GraphicsTest();
#else
	{

		editor::MapFileManager::GetSingletonInstance().LoadMapFile("InWanderLand.pmap");
		ContentsLayer::PlayContents();

		//ShortcutSystem::Instance();

		//{
		//    auto obj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		//    //obj->GetTransform()->SetLocalPosition(Vector3d{ -500,500,1 });
		//    obj->GetTransform()->SetLocalScale(Vector3d{ 100,100,100 });
		//    auto text = obj->AddComponent<yunutyEngine::graphics::UIText>();
		//    text->GetGI().SetFontSize(20);
		//    auto test = obj->AddComponent<TestComponent2>();
		//    test->text = text;
		//}

		//auto rsrcMgr = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();

		//auto sphereMesh = rsrcMgr->GetMesh(L"Sphere");
		//auto mouseCursorObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		//RegisterToEditorObjectContainer(mouseCursorObject);
		//auto mouseCursorMesh = mouseCursorObject->AddComponent<yunutyEngine::graphics::StaticMeshRenderer>();
		//mouseCursorMesh->GetGI().SetMesh(sphereMesh);
		//mouseCursorMesh->GetGI().GetMaterial()->SetColor(yunuGI::Color{ 0, 0, 0, 1 });

		///*WarriorProductor::Instance().CreateUnit(Vector3d(0.0f, 0.0f, 0.0f));;
		//MagicianProductor::Instance().CreateUnit(Vector3d(0.0f, 0.0f, 2.0f));;
		//HealerProductor::Instance().CreateUnit(Vector3d(0.0f, 0.0f, -2.0f));*/

		//auto skillPreviewCubeMeshObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		//AttachDebugMesh(skillPreviewCubeMeshObject, DebugMeshType::Cube)->GetGI().SetMaterial(0, GetColoredDebugMaterial(yunuGI::Color::red(), false));
		//SkillPreviewSystem::Instance().SetPathPreviewObject(skillPreviewCubeMeshObject);
		//RegisterToEditorObjectContainer(skillPreviewCubeMeshObject);

		//auto skillPreviewSphereMeshObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
		//AttachDebugMesh(skillPreviewSphereMeshObject, DebugMeshType::Sphere)->GetGI().SetMaterial(0, GetColoredDebugMaterial(yunuGI::Color::red(), false));
		//SkillPreviewSystem::Instance().SetRangePreviewObject(skillPreviewSphereMeshObject);
		//RegisterToEditorObjectContainer(skillPreviewSphereMeshObject);

		//InputManager::Instance();
		//UIManager::Instance();
		//bool mapFound = false;

		//editor::MapFileManager::GetSingletonInstance().LoadMapFile("InWanderLand.pmap");
		//editor::InstanceManager::GetSingletonInstance().ApplyInstancesAsPlaytimeObjects();

}
#endif
#endif
}

void application::contents::ContentsLayer::Update(float ts)
{
	std::cout << Time::GetFPS() << std::endl;
}

void application::contents::ContentsLayer::GUIProgress()
{

}

void application::contents::ContentsLayer::Finalize()
{

}

void application::contents::ContentsLayer::PlayContents()
{
    UIManager::Instance().ImportUI("InWanderLand.iwui");
	SingletonInstanceContainer::SingleInstance().PermitCreateInstances();
	editor::InstanceManager::GetSingletonInstance().ApplyInstancesAsPlaytimeObjects();
	InputManager::Instance().SetInputManagerActive(true);
	GameManager::Instance().Reset();

	for (auto e : componentsCreatedByEditorVector)
	{
		e->SetActive(true);
	}

	//InputManager::Instance();
	//UIManager::Instance();

	/// Editor 에서 수정하여 Map Data 에 저장할 부분

	/// 카메라의 경우 CameraData 의 ApplyInstancesAsPlaytimeObjects 에서 처리함

	auto skillPreviewCubeMeshObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	AttachDebugMesh(skillPreviewCubeMeshObject, DebugMeshType::Cube)->GetGI().SetMaterial(0, GetColoredDebugMaterial(yunuGI::Color::red(), false));
	SkillPreviewSystem::Instance().SetPathPreviewObject(skillPreviewCubeMeshObject);

	auto skillPreviewSphereMeshObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	AttachDebugMesh(skillPreviewSphereMeshObject, DebugMeshType::Sphere)->GetGI().SetMaterial(0, GetColoredDebugMaterial(yunuGI::Color::red(), false));
	SkillPreviewSystem::Instance().SetRangePreviewObject(skillPreviewSphereMeshObject);

	/// 임시
	//RegisterToEditorObjectVector(MagicianProductor::Instance().CreateUnit(Vector3d(-7.0f, 0.0f, -7.0f))->GetGameObject());
	//RegisterToEditorObjectVector(HealerProductor::Instance().CreateUnit(Vector3d(-7.0f, 0.0f, 7.0f))->GetGameObject());
	//RegisterToEditorObjectVector(BossProductor::Instance().CreateUnit(Vector3d(-7.0f, 0.0f, 7.0f))->GetGameObject());
	//RegisterToEditorObjectVector(BossProductor::Instance().CreateUnit(Vector3d(-7.0f, 0.0f, 7.0f))->GetGameObject());

	auto rsrcMgr = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();

    /// Playable 동작들을 일괄 처리할 부분입니다.
    PlayableComponent::OnGameStartAll();
}

void application::contents::ContentsLayer::PauseContents()
{
	Time::SetTimeScale(FLT_MIN * 1000);
	for (auto e : componentsCreatedByEditorVector)
	{
		e->SetActive(false);
	}

    /// Playable 동작들을 일괄 처리할 부분입니다.
    PlayableComponent::OnGamePauseAll();
}

void application::contents::ContentsLayer::ResumeContents()
{
	Time::SetTimeScale(1);
	for (auto e : componentsCreatedByEditorVector)
	{
		e->SetActive(true);
	}

    /// Playable 동작들을 일괄 처리할 부분입니다.
    PlayableComponent::OnGameResumeAll();
}

void application::contents::ContentsLayer::StopContents()
{
	Time::SetTimeScale(1);
	isStoppedOnce = true;
	ClearPlaytimeObject();
	ShortcutSystem::Instance().Clear();
	for (auto e : componentsCreatedByEditorVector)
	{
		e->SetActive(false);
	}

    UIManager::Instance().Clear();

    /// Playable 동작들을 일괄 처리할 부분입니다.
    PlayableComponent::OnGameStopAll();
}

#ifdef GEN_TESTS
void application::contents::ContentsLayer::AssignTestInitializer(std::function<void()> testInitializer)
{
	ContentsLayer::testInitializer = testInitializer;
	YunutyCycle::SingleInstance().onExceptionThrown = [](const std::exception& e) {
		application::Application::GetInstance().AddMainLoopTodo([=]() {
			Assert::Fail(yunutyEngine::yutility::GetWString(e.what()).c_str());
			});
	};
}
#endif

void application::contents::ContentsLayer::ClearPlaytimeObject()
{
	/// 카메라가 추적하고 있는 
	/// 생성된 모든 게임 플레이 오브젝트 삭제
	for (auto e : objectCreatedByEditorList)
	{
		//      if (e->getName() == "")
			  //e->SetSelfActive(false);
		yunutyEngine::Scene::getCurrentScene()->DestroyGameObject(e);
	}

	for (auto e : componentsCreatedByEditorVector)
	{
		e->SetActive(false);
	}
	objectCreatedByEditorList.clear();

	SingletonInstanceContainer::SingleInstance().ClearSingletonInstances();
}

void application::contents::ContentsLayer::ShortcutInit()
{
	auto& scsys = ShortcutSystem::Instance();
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::D, false } },
		[=]()
		{
			DebugGraphic::SetDebugGraphicsEnabled(!DebugGraphic::AreDebugGraphicsEnabled());
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::NUM_1, false } },
		[=]()
		{
			for (auto& each : yunutyEngine::Scene::getCurrentScene()->GetChildren())
			{
				auto comp = each->GetComponent<Unit>();
				if (comp == nullptr)
				{
					continue;
				}

				if (comp->GetUnitSide() == Unit::UnitSide::Player)
				{
					auto& scsysIns = ShortcutSystem::Instance();
					comp->GetGameObject()->SetSelfActive(scsysIns.GetTriggerSwitch(scsysIns.GetKeyIndex({ { KeyCode::Control, true }, { KeyCode::NUM_1, false } })));
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::NUM_2, false } },
		[=]()
		{
			for (auto& each : yunutyEngine::Scene::getCurrentScene()->GetChildren())
			{
				auto comp = each->GetComponent<Unit>();
				if (comp == nullptr)
				{
					continue;
				}

				if (comp->GetUnitSide() == Unit::UnitSide::Enemy)
				{
					auto& scsysIns = ShortcutSystem::Instance();
					comp->GetGameObject()->SetSelfActive(scsysIns.GetTriggerSwitch(scsysIns.GetKeyIndex({ { KeyCode::Control, true }, { KeyCode::NUM_2, false } })));
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::NUM_3, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				auto& scsysIns = ShortcutSystem::Instance();
				each->GetPaletteInstance()->GetGameObject()->SetSelfActive(scsysIns.GetTriggerSwitch(scsysIns.GetKeyIndex({ { KeyCode::Control, true }, { KeyCode::NUM_3, false } })));
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::NUM_4, false } },
		[=]()
		{
			for (auto& each : yunutyEngine::Scene::getCurrentScene()->GetChildren())
			{
				auto comp = each->GetComponent<yunutyEngine::graphics::PointLight>();
				if (comp == nullptr)
				{
					continue;
				}

				auto& scsysIns = ShortcutSystem::Instance();
				comp->GetGameObject()->SetSelfActive(scsysIns.GetTriggerSwitch(scsysIns.GetKeyIndex({ { KeyCode::Control, true }, { KeyCode::NUM_4, false } })));
			}
		});

	/// Ornament 개별 적용
#pragma region
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::NUM_1, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Bush_001")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::NUM_2, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Bush_002")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::NUM_3, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Stone_001")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::NUM_4, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Stone_002")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::NUM_5, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Grass_001")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::NUM_6, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Grass_002")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::NUM_7, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_LeafShadow")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::NUM_8, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Pebbles_001")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::NUM_9, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Book01")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::NUM_0, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Book03")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Q, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Book05")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::W, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Book06")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Alphabet_E, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Bookshelf01")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::R, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Bookshelf02")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::T, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Bookshelf03")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Y, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Bookshelf04")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::U, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Mushroom01")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::I, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Mushroom02")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::O, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_BigTree_001")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::P, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_BossCorrider_Floor")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::A, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Candle")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::S, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_CastleWall")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::D, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_CastleWall_Door")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::F, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_CastleWall_Pillar")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::G, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Chair")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::H, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Corrider_Floor")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::J, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Corrider_Floor06")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::K, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_CupTower")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::L, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Entrance_Floor")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Z, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_FloorBrick")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::X, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Fork")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::C, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_GuideBook")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::V, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Hall_Corrider_Floor")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::B, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Hall_Entrance_Floor")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::N, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Hall_Floor")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::M, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Hat01")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Alt, true }, { KeyCode::NUM_1, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Hat02")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Alt, true }, { KeyCode::NUM_2, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Key_Room_Floor")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Alt, true }, { KeyCode::NUM_3, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Lamp")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Alt, true }, { KeyCode::NUM_4, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Pillar")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Alt, true }, { KeyCode::NUM_5, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Room_Floor01")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Alt, true }, { KeyCode::NUM_6, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_SmallBush_001")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Alt, true }, { KeyCode::NUM_7, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Stage1_Floor")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Alt, true }, { KeyCode::NUM_8, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Stump")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Alt, true }, { KeyCode::NUM_9, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Temple_Book_etc")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Alt, true }, { KeyCode::NUM_0, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Temple_Books")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Alt, true }, { KeyCode::Q, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Temple_Floor")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Alt, true }, { KeyCode::W, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Temple_Pillar")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Alt, true }, { KeyCode::Alphabet_E, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Temple_Pillar_Broken")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Alt, true }, { KeyCode::R, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Temple_Rabbit")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Alt, true }, { KeyCode::T, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Temple_Welcome")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Alt, true }, { KeyCode::Y, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Trunk_001")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Alt, true }, { KeyCode::U, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_VertexColor")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Alt, true }, { KeyCode::I, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Wall_3m")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Alt, true }, { KeyCode::O, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Wall_7m")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Alt, true }, { KeyCode::P, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Wall_21m")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
	scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::LShift, true }, { KeyCode::Alt, true }, { KeyCode::A, false } },
		[=]()
		{
			for (auto& each : editor::InstanceManager::GetSingletonInstance().GetList<editor::OrnamentData>())
			{
				if (each->pod.templateData->pod.staticFBXName == "SM_Arch_Window_7m")
				{
					auto& scsysIns = ShortcutSystem::Instance();
					each->GetPaletteInstance()->GetGameObject()->SetSelfActive(!each->GetPaletteInstance()->GetGameObject()->GetSelfActive());
				}
			}
		});
#pragma endregion Ornament Shortcut
}

void application::contents::ContentsLayer::RegisterToEditorObjectContainer(GameObject* p_obj)
{
	objectCreatedByEditorList.push_back(p_obj);
}

void application::contents::ContentsLayer::RegisterToEditorComponentVector(Component* p_com)
{
	componentsCreatedByEditorVector.push_back(p_com);
}
