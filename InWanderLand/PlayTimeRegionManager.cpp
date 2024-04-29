#include "PlayTimeRegionManager.h"
#include "RegionData.h"
#include "OrnamentData.h"
#include "SpecialEvent.h"
#include "PlayerController.h"
#include "SingleNavigationField.h"
#include "GameManager.h"

void PlayTimeRegionManager::Start()
{
	for (auto e : stage2Ornaments)
	{
		e->SetSelfActive(false);
	}
}

void PlayTimeRegionManager::AddRegionData(application::editor::RegionData* p_regionData)
{
	switch (static_cast<SpecialEventType>(p_regionData->pod.specialEvent))
	{
		case SpecialEventType::Stage2StartRegion :
		{
			stage2StartPosition = Vector3d(p_regionData->pod.x, 0, p_regionData->pod.z);
		}
		break;

		case SpecialEventType::Stage1To2Transition:
		{
			stage1ToStage2Function = [=]()
				{
					for (auto e : PlayerController::SingleInstance().GetPlayerMap())
					{
						e.second->GetGameObject()->GetComponent<yunutyEngine::NavigationAgent>()->SetActive(false);
						e.second->GetTransform()->SetWorldPosition({ stage2StartPosition });
						e.second->GetGameObject()->GetComponent<yunutyEngine::NavigationAgent>()->SetActive(true);
						e.second->GetGameObject()->GetComponent<yunutyEngine::NavigationAgent>()->AssignToNavigationField(&SingleNavigationField::Instance());
						e.second->GetGameObject()->GetComponent<yunutyEngine::NavigationAgent>()->Relocate(stage2StartPosition);
						e.second->GetGameObject()->GetComponent<yunutyEngine::NavigationAgent>()->MoveTo(stage2StartPosition);
						e.second->SetUnitStateIdle();
						e.second->StopMove();
					}
					//PlayerController::SingleInstance().GetPlayerMap();
					cameraDotween->DOMove(stage2StartPosition + Vector3d(0, 25, -20), 2.0f);

					for (auto e : stage2Ornaments)
					{
						e->SetSelfActive(true);
					}

					for (auto e : stage1Ornaments)
					{
						e->SetSelfActive(false);
					}

				};
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
