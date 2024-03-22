#include "PlaytimeRegion.h"
#include "Unit.h"
#include "RegionData.h"
#include "SpecialEvent.h"
#include "PlayerController.h"

PlaytimeRegion::~PlaytimeRegion()
{
	regionData->playtimeRegion = nullptr;
}

void PlaytimeRegion::Start()
{
	if (static_cast<SpecialEventType>(regionData->pod.specialEvent) == SpecialEventType::Stage2StartRegion)
	{
		stage2StartPosition = Vector3d(regionData->pod.x, 0, regionData->pod.z);
	}

	stage1ToStage2Function = [=]()
		{
			for (auto e : PlayerController::SingleInstance().GetPlayerMap())
			{
				e.second->GetTransform()->SetWorldPosition({ stage2StartPosition });
			}
			//PlayerController::SingleInstance().GetPlayerMap();
		};
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
		for (auto& each : OnEnter)
		{
			each();
		}
		switch (static_cast<SpecialEventType>(regionData->pod.specialEvent))
		{
			case SpecialEventType::Stage1To2Transition :
				stage1ToStage2Function();
				break;
		}
		// 가려야 하는 장식물들을 가리는 부분
	}
}
void PlaytimeRegion::OnTriggerExit(physics::Collider* collider)
{
	// 조건 더 다듬을 것
	if (Unit* colliderUnitComponent = collider->GetGameObject()->GetComponent<Unit>();
		colliderUnitComponent != nullptr && colliderUnitComponent->GetUnitSide() == Unit::UnitSide::Player)
	{
		for (auto& each : OnLeave)
		{
			each();
		}
		// 가려져 있던 장식물들을 다시 보이게 하는 부분
	}
}
