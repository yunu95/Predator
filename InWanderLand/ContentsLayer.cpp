#include "InWanderLand.h"
#include "ContentsLayer.h"

#include "YunutyEngine.h"
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

#include "PlayerSkillManager.h"
#include "ShortcutSystem.h"
#include "RobinSkillDevelopmentSystem.h"
#include "ScriptSystem.h"
#include "Script.h"
#include "UIImage.h"
#include "PlayableComponent.h"
#include "CinematicManager.h"
#include "TutorialManager.h"
#include "ContentsObserver.h"
#include "ParticleTool_Manager.h"
#include "UVAnimator.h"
#include "VFXAnimator.h"
#include "InitialLoadingScreen.h"
#include "InstanceManager.h"
#include "ChessPool.h"

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

class TestComponent3 : public yunutyEngine::Component
{
public:
    yunutyEngine::graphics::UIText* text_FPS;
    yunutyEngine::graphics::UIText* text_update;
    yunutyEngine::graphics::UIText* text_physx;
    yunutyEngine::graphics::UIText* text_Render;
    virtual void Update() override
    {
        std::wstring temp;
        temp = std::to_wstring(Time::GetFPS());
        text_FPS->GetGI().SetText(L"fps : " + temp);
        /*temp = std::to_wstring(Time::GetTimeUsedForUpdate());
        text_update->GetGI().SetText(L"time used for update : " + temp);
        temp = std::to_wstring(Time::GetTimeUsedForPhysx());
        text_physx->GetGI().SetText(L"time used for physx : " + temp);
        temp = std::to_wstring(Time::GetTimeUsedForRender());
        text_Render->GetGI().SetText(L"time used for render : " + temp);*/
        if (DebugGraphic::AreDebugGraphicsEnabled())
        {
            temp = std::to_wstring(100 * Time::GetTimeUsedForUpdate() / Time::GetDeltaTimeUnscaled());
            text_update->GetGI().SetText(L"time used for update : " + temp + L"%");
            temp = std::to_wstring(100 * Time::GetTimeUsedForPhysx() / Time::GetDeltaTimeUnscaled());
            text_physx->GetGI().SetText(L"time used for physx : " + temp + L"%");
            temp = std::to_wstring(100 * Time::GetTimeUsedForRender() / Time::GetDeltaTimeUnscaled());
            text_Render->GetGI().SetText(L"time used for render : " + temp + L"%");
        }
        text_FPS->SetActive(DebugGraphic::AreDebugGraphicsEnabled());
        text_update->SetActive(DebugGraphic::AreDebugGraphicsEnabled());
        text_physx->SetActive(DebugGraphic::AreDebugGraphicsEnabled());
        text_Render->SetActive(DebugGraphic::AreDebugGraphicsEnabled());
    }
};

class TestComponent4 : public yunutyEngine::Component
{
public:
    yunutyEngine::graphics::Animator* anim;
    yunuGI::IAnimation* idle;
    yunuGI::IAnimation* walk;
    virtual void Update() override
    {
        if (Input::isKeyPushed(yunutyEngine::KeyCode::LeftArrow))
        {
            anim->ChangeAnimation(idle, 0.31, 1);
        }

        if (Input::isKeyPushed(yunutyEngine::KeyCode::RightArrow))
        {
            anim->ChangeAnimation(walk, 0.31, 1);
        }

        if (Input::isKeyPushed(yunutyEngine::KeyCode::UpArrow))
        {
            Time::SetTimeScale(0.1);
        }
    }
};

/// 그래픽스 테스트용
void GraphicsTest()
{
    auto camObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    camObj->AddComponent<tests::GraphicsTestCam>();
    camObj->GetComponent<tests::GraphicsTestCam>()->GetGI().SetAsMain();

    camObj->GetTransform()->SetLocalPosition(Vector3d{ 0,0,-5 });
    //camObj->GetTransform()->SetLocalRotation(Quaternion{ Vector3d{ 0,0,0 } });

    auto skillPreviewSystem = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    auto systemComponent = skillPreviewSystem->AddComponent<SkillPreviewSystem>();
    systemComponent->Init();
    systemComponent->camObj = camObj;

    const yunuGI::IResourceManager* _resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
    //_resourceManager->LoadFile("FBX/VFX");
    auto& animationList = _resourceManager->GetAnimationList();
    yunuGI::IAnimation* animation = nullptr;
    yunuGI::IAnimation* animation2 = nullptr;
    yunuGI::IAnimation* animation3 = nullptr;

    for (auto& i : animationList)
    {
        if (i->GetName() == L"Rig_Robin_arpbob|Ani_Robin_Idle")
        {
            animation = i;
            animation->SetLoop(true);
        }

        if (i->GetName() == L"Rig_Robin_arpbob|Ani_Robin_Walk")
        {
            animation2 = i;
            animation2->SetLoop(true);
        }
        if (i->GetName() == L"Rig_Robin_arpbob|Ani_Robin_Skill1")
        {
            animation3 = i;
        }
    }

    {
        auto obj2 = Scene::getCurrentScene()->AddGameObject();
        auto test = obj2->AddComponent<TestComponent4>();

        auto obj = Scene::getCurrentScene()->AddGameObjectFromFBX("SKM_Robin");
        auto anim = obj->GetComponent<yunutyEngine::graphics::Animator>();
        anim->PushAnimation(animation);
        anim->PushAnimation(animation2);
        anim->Play(animation);

        test->anim = anim;
        test->idle = animation;
        test->walk = animation2;
    }

    /*{
        auto obj = Scene::getCurrentScene()->AddGameObjectFromFBX("SM_Stage1_Floor_01");

    }

    {
        auto obj = Scene::getCurrentScene()->AddGameObjectFromFBX("VFX_Monster2_Skill_Preview");
        obj->AddComponent<VFXAnimator>();
        obj->GetTransform()->SetLocalPosition(Vector3d{ 0,0,0 });
    }
    {
        auto obj = Scene::getCurrentScene()->AddGameObjectFromFBX("SM_Temple_Floor");
        obj->GetTransform()->SetLocalPosition(Vector3d{ 0,0,-20 });
        auto test = obj->AddComponent<TestComponent4>();
        test->sys = systemComponent;
    }*/

    yunutyEngine::graphics::Renderer::SingleInstance().SortByCameraDirection();
    //yunutyEngine::graphics::Renderer::SingleInstance().SetUseIBL(true);
    //yunutyEngine::graphics::Renderer::SingleInstance().SortByCameraDirection();
}

void application::contents::ContentsLayer::SetInputControl(bool control)
{
    contentsInputControl = control;
}

bool application::contents::ContentsLayer::GetInputControl()
{
    return contentsInputControl;
}
class ContentsInitializer : public yunutyEngine::Component
{
    coroutine::Coroutine Initialize()
    {
        chrono::steady_clock::time_point base = chrono::high_resolution_clock::now();
        wanderUtils::ResourceRecursiveLoader::Load("./", { ".cso" });
        wanderUtils::ResourceRecursiveLoader::Load("Texture/LoadingScreen/");
        wanderUtils::ResourceRecursiveLoader::Load("sounds/LoadingScreen/");
        auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(chrono::high_resolution_clock::now() - base);
        float timeTookForLoadingLoadingScreen = dur.count() / 1000.0f;

        auto tempCam = Scene::getCurrentScene()->AddGameObject()->AddComponent<graphics::Camera>();
        auto logoObj = Scene::getCurrentScene()->AddGameObject()->AddComponent<InitialLoadingScreen>();
        tempCam->SetCameraMain();

        auto loadingCoroutine = StartCoroutine(wanderUtils::ResourceRecursiveLoader::LoadByCoroutine());
        while (!loadingCoroutine.expired())
        {
            co_await std::suspend_always{};
        }

        /// Particle 및 AnimationEvent
        auto& particleManager = application::particle::ParticleTool_Manager::GetSingletonInstance();
        particleManager.LoadSkinnedFBX();
        particleManager.LoadPP("InWanderLand.pp");
        particleManager.LoadPPIs("InWanderLand.ppis");
        {
            auto obj = Scene::getCurrentScene()->AddGameObject();
            auto test3 = obj->AddComponent<TestComponent3>();
            {
                auto textObj = obj->AddGameObject();
                auto text = textObj->AddComponent<yunutyEngine::graphics::UIText>();
                text->GetGI().SetFontSize(30);
                text->GetGI().SetColor(yunuGI::Color{ 1,0,1,1 });
                textObj->GetTransform()->SetLocalScale(Vector3d{ 1200,500,0 });
                textObj->GetTransform()->SetLocalPosition(Vector3d{ 0,-0,0 });
                test3->text_FPS = text;
            }
            {
                auto textObj = obj->AddGameObject();
                auto text = textObj->AddComponent<yunutyEngine::graphics::UIText>();
                text->GetGI().SetFontSize(30);
                text->GetGI().SetColor(yunuGI::Color{ 1,0,1,1 });
                textObj->GetTransform()->SetLocalScale(Vector3d{ 1200,500,0 });
                textObj->GetTransform()->SetLocalPosition(Vector3d{ 0,30,0 });
                test3->text_update = text;
            }
            {
                auto textObj = obj->AddGameObject();
                auto text = textObj->AddComponent<yunutyEngine::graphics::UIText>();
                text->GetGI().SetFontSize(30);
                text->GetGI().SetColor(yunuGI::Color{ 1,0,1,1 });
                textObj->GetTransform()->SetLocalScale(Vector3d{ 1200,500,0 });
                textObj->GetTransform()->SetLocalPosition(Vector3d{ 0,60,0 });
                test3->text_physx = text;
            }
            {
                auto textObj = obj->AddGameObject();
                auto text = textObj->AddComponent<yunutyEngine::graphics::UIText>();
                text->GetGI().SetFontSize(30);
                text->GetGI().SetColor(yunuGI::Color{ 1,0,1,1 });
                textObj->GetTransform()->SetLocalScale(Vector3d{ 1200,500,0 });
                textObj->GetTransform()->SetLocalPosition(Vector3d{ 0,90,0 });
                test3->text_Render = text;
            }
        }

#ifndef EDITOR
#ifdef GRAPHICS_TEST
        {
            yunutyEngine::Collider2D::SetIsOnXYPlane(false);
            Quaternion inGameRot;
            inGameRot.x = 0.860572696;
            inGameRot.y = 0.0646217689;
            inGameRot.z = 0.11142046;
            inGameRot.w = 0.492771924;
            auto directionalLight = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
            directionalLight->GetTransform()->SetLocalRotation(inGameRot);
            directionalLight->GetTransform()->SetLocalPosition(Vector3d{ 0,0,-20 });
            auto light = directionalLight->AddComponent<yunutyEngine::graphics::DirectionalLight>();
            auto color = yunuGI::Color{ 1,1,1,1.f };
            light->GetGI().SetLightDiffuseColor(color);
        }
        GraphicsTest();
#else
        {
            UIManager::Instance().ImportUI("InWanderLand.iwui");
            UIManager::Instance().GetUIElementByEnum(UIEnumID::BlackMask_LeftToRight)->EnableElement();
            UIManager::Instance().GetUIElementByEnum(UIEnumID::BlackMask_LeftToRight)->DisableElement();
            //static_cast<application::contents::ContentsLayer*>(application::Application::GetInstance().layers[(int)application::Application::LayerList::ContentsLayer])->PlayContents();
        }
#endif
#endif
#ifndef EDITOR
        DebugGraphic::SetDebugGraphicsEnabled(false);
#endif

        SkillPreviewSystem::Instance().Init();
        Scene::getCurrentScene()->DestroyGameObject(GetGameObject());
        co_return;
    }
    virtual void Start() override
    {
        StartCoroutine(Initialize());
    }
};
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
    //TutorialManager::Instance();
    Scene::getCurrentScene()->AddGameObject()->AddComponent<ContentsInitializer>();
    //SkillPreviewSystem::Instance().Init();	
}

void application::contents::ContentsLayer::Update(float ts)
{
    //std::cout << Time::GetFPS() << std::endl;
}

void application::contents::ContentsLayer::GUIProgress()
{

}

void application::contents::ContentsLayer::Finalize()
{

}

void application::contents::ContentsLayer::PlayContents(ContentsPlayFlag playFlag)
{
    if (IsPlayingContents())
    {
        return;
    }
    playingContents = true;
    if (bool(playFlag & ContentsPlayFlag::ImportUI))
    {
        UIManager::Instance().ImportUI("InWanderLand.iwui");
    }

    editor::InstanceManager::GetSingletonInstance().ApplyInstancesAsPlaytimeObjects();

    yunutyEngine::graphics::Renderer::SingleInstance().SortByCameraDirection();

    PlayerController::Instance().Reset();

    auto rsrcMgr = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();

    ContentsObserver::Instance().OnPlayContents();
    SkillPreviewSystem::Instance().camObj = RTSCam::Instance().GetGameObject();
    TacticModeSystem::Instance();

    /// Playable 동작들을 일괄 처리할 부분입니다.
    PlayableComponent::OnGameStartAll();

    wanderUtils::ChangeStageToOne();
}

void application::contents::ContentsLayer::PauseContents()
{
    Time::SetTimeScale(FLT_MIN * 1000);

    /// Playable 동작들을 일괄 처리할 부분입니다.
    PlayableComponent::OnGamePauseAll();
}

void application::contents::ContentsLayer::ResumeContents()
{
    Time::SetTimeScale(1);

    /// Playable 동작들을 일괄 처리할 부분입니다.
    PlayableComponent::OnGameResumeAll();
}

void application::contents::ContentsLayer::StopContents(ContentsStopFlag stopFlag)
{
    if (!IsPlayingContents())
    {
        return;
    }
    Time::SetTimeScale(1);
    isStoppedOnce = true;
    playingContents = false;
    ShortcutSystem::Instance().Clear();

    yunutyEngine::graphics::Renderer::SingleInstance().SetUseIBL(true);
    yunutyEngine::graphics::Renderer::SingleInstance().SetLightMap(L"Stage1LightMap");

    ContentsObserver::Instance().OnStopContents();

    /// Playable 동작들을 일괄 처리할 부분입니다.
    PlayableComponent::OnGameStopAll();

    if (bool(stopFlag & ContentsStopFlag::ClearUI))
        UIManager::Instance().Clear();

    wanderUtils::ClearContentsCallbacks();
    SFXManager::SingleInstance().SetSFXVolume(1);
    SoundSystem::UnmuteMusic();
}

bool application::contents::ContentsLayer::IsPlayingContents() const
{
    return playingContents;
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

void application::contents::ContentsLayer::ShortcutInit()
{
    auto& scsys = ShortcutSystem::Instance();
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::D, false } },
        [=]()
        {
            DebugGraphic::SetDebugGraphicsEnabled(!DebugGraphic::AreDebugGraphicsEnabled());
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::Alt, true }, { KeyCode::L, false } },
        [=]()
        {
            ProgressManager::SingleInstance().LoadCheckPoint();
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::Alt, true }, { KeyCode::C, false } },
        [=]()
        {
            ProgressManager::SingleInstance().NotifyCurrentProgressSaveAll();
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::Alt, true }, { KeyCode::M, false } },
        [=]()
        {
            for (auto& [uiID, skillUpgradeType] : PlayerController::Instance().skillUpgradeByUI)
            {
                PlayerController::Instance().SetSkillUpgradeTarget(uiID);
                PlayerController::Instance().UpgradeSkill();
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::Alt, true }, { KeyCode::O, false } },
        [=]()
        {
            if (PlayerController::Instance().IsOPMode())
            {
                PlayerController::Instance().DeactivateOPMode();
            }
            else
            {
                PlayerController::Instance().ActivateOPMode();
            }
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::Alt, true }, { KeyCode::T, false } },
        [=]()
        {
            static float stoppedTimeScale = 1;
            stoppedTimeScale = stoppedTimeScale < 1 ? 1 : 0.0001f;
            Time::SetTimeScale(stoppedTimeScale);
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::Alt, true }, { KeyCode::DownArrow, false } },
        [=]()
        {
            Time::SetTimeScale(std::fmaxf(0.01, Time::GetTimeScale() - 0.09));
        });
    scsys.RegisterUniqueTrigger({ { KeyCode::Control, true }, { KeyCode::Alt, true }, { KeyCode::UpArrow, false } },
        [=]()
        {
            Time::SetTimeScale(Time::GetTimeScale() + 0.09);
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

                if (!comp->IsPlayerUnit())
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

                if (!comp->IsPlayerUnit())
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


