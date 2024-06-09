#include "PlayTimeRegionManager.h"
#include "RegionData.h"
#include "OrnamentData.h"
#include "SpecialEvent.h"
#include "PlayerController.h"
#include "SingleNavigationField.h"


void PlayTimeRegionManager::Start()
{
    for (auto e : stage2Ornaments)
    {
        e->SetSelfActive(false);
    }
}

void PlayTimeRegionManager::OnContentsStop()
{
    this->SetActive(false);
    stage1ToStage2Function = nullptr;
    cameraDotween = nullptr;
    stage1Ornaments.clear();
    stage2Ornaments.clear();
}

void PlayTimeRegionManager::AddRegionData(application::editor::RegionData* p_regionData)
{
    switch (static_cast<SpecialEventType>(p_regionData->pod.specialEvent))
    {
    case SpecialEventType::Stage2StartRegion:
    {
        stage2StartPosition = Vector3d(p_regionData->pod.x, 0, p_regionData->pod.z);
    }
    break;

    default:
        break;
    }
}

void PlayTimeRegionManager::RegisterOrnament(GameObject* p_obj, int p_stageNum)
{
    if (p_stageNum == 1)
    {
        stage1Ornaments.push_back(p_obj);
    }
    else if (p_stageNum == 2)
    {
        stage2Ornaments.push_back(p_obj);
    }
}

void PlayTimeRegionManager::SetCameraDotween(Dotween* p_dot)
{
    cameraDotween = p_dot;
}
