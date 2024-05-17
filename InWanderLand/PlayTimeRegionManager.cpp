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

		case SpecialEventType::Stage1To2Transition:
		{
			stage1ToStage2Function = [=]()
				{
					for (auto e : PlayerController::Instance().GetPlayerMap())
					{
						e.second->m_navAgentComponent->SetActive(false);
						e.second->GetTransform()->SetWorldPosition({ stage2StartPosition });
						e.second->m_navAgentComponent->SetActive(true);
						e.second->m_navAgentComponent->AssignToNavigationField(&SingleNavigationField::Instance());
						e.second->m_navAgentComponent->Relocate(stage2StartPosition);
						e.second->m_navAgentComponent->MoveTo(stage2StartPosition);
						e.second->SetUnitStateIdle();
						e.second->StopMove();


						e.second->m_navAgentComponent->SetActive(false);
						e.second->GetTransform()->SetWorldPosition({ stage2StartPosition });
						e.second->m_navAgentComponent->SetActive(true);
						e.second->m_navAgentComponent->AssignToNavigationField(&SingleNavigationField::Instance());
						e.second->m_navAgentComponent->Relocate(stage2StartPosition);
						e.second->m_navAgentComponent->MoveTo(stage2StartPosition);
						e.second->SetUnitStateIdle();
						e.second->StopMove();
					}
					//PlayerController::Instance().GetPlayerMap();
					cameraDotween->DOMove(stage2StartPosition + Vector3d(0, 25, -20), 2.0f);

					for (auto e : stage2Ornaments)
					{
						e->SetSelfActive(true);
					}

					for (auto e : stage1Ornaments)
					{
						e->SetSelfActive(false);
					}

					yunutyEngine::graphics::Renderer::SingleInstance().SetUseIBL(false);
					yunutyEngine::graphics::Renderer::SingleInstance().SetLightMap(L"Stage2LightMap");
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
