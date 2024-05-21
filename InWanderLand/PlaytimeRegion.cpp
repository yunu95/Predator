#include "PlaytimeRegion.h"
#include "Unit.h"
#include "RegionData.h"
#include "SpecialEvent.h"
#include "PlayerController.h"
#include "SingleNavigationField.h"
#include "PlayTimeRegionManager.h"
#include "Application.h"
#include "ContentsLayer.h"

PlaytimeRegion::~PlaytimeRegion()
{
    regionData->playtimeRegion = nullptr;
    for (auto each : regionData->GetDisablingOrnaments())
    {
        if (auto instance = each->GetPaletteInstance())
        {
            instance->GetGameObject()->SetSelfActive(true);
        }
    }
}

void PlaytimeRegion::Start()
{
    //application::contents::ContentsLayer* contentsLayer = dynamic_cast<application::contents::ContentsLayer*>(application::Application::GetInstance().GetContentsLayer());
    //contentsLayer->RegisterToEditorComponentVector(this);
}

//void PlaytimeRegion::SetRegionName(std::wstring p_name)
//{
//	//m_regionName = p_name;
//}

void PlaytimeRegion::OnTriggerEnter(physics::Collider* collider)
{
    // 조건 더 다듬을 것
    if (Unit* colliderUnitComponent = collider->GetGameObject()->GetComponent<Unit>();
        colliderUnitComponent != nullptr && colliderUnitComponent->GetUnitSide() == Unit::UnitSide::Player)
    {
        enteredPlayerColliders.insert(collider);
        if (!isOnceActivated)
        {
            for (auto& each : OnEnter)
            {
                each();
            }
            switch (static_cast<SpecialEventType>(regionData->pod.specialEvent))
            {
            case SpecialEventType::Stage1To2Transition:
                PlayTimeRegionManager::Instance().stage1ToStage2Function();
                break;
            }
        }
        // 가려야 하는 장식물들을 가리는 부분
        for (auto each : regionData->GetDisablingOrnaments())
        {
            if (auto instance = each->GetPaletteInstance())
            {
                instance->GetGameObject()->SetSelfActive(false);
            }
        }

        isOnceActivated = true;
    }
}
void PlaytimeRegion::OnTriggerExit(physics::Collider* collider)
{
    // 조건 더 다듬을 것
    if (Unit* colliderUnitComponent = collider->GetGameObject()->GetComponent<Unit>();
        colliderUnitComponent != nullptr && colliderUnitComponent->GetUnitSide() == Unit::UnitSide::Player)
    {
        enteredPlayerColliders.erase(collider);
        for (auto& each : OnLeave)
        {
            each();
        }
        if (enteredPlayerColliders.empty())
        {
            // 가려져 있던 장식물들을 다시 보이게 하는 부분
            for (auto each : regionData->GetDisablingOrnaments())
            {
                if (auto instance = each->GetPaletteInstance())
                {
                    instance->GetGameObject()->SetSelfActive(true);
                }
            }
        }
    }
}
