#include "PlaytimeRegion.h"
#include "Unit.h"
#include "RegionData.h"

PlaytimeRegion::~PlaytimeRegion()
{
	regionData->playtimeRegion = nullptr;
}
//void PlaytimeRegion::SetRegionName(std::wstring p_name)
//{
//	//m_regionName = p_name;
//}

void PlaytimeRegion::OnTriggerEnter(physics::Collider* collider)
{
	// 조건 더 다듬을 것
	if (Unit* colliderUnitComponent = collider->GetGameObject()->GetComponent<Unit>();
		colliderUnitComponent->GetUnitSide() == Unit::UnitSide::Player)
	{
		for (auto& each : OnEnter)
		{
			each();
		}
		// 가려야 하는 장식물들을 가리는 부분
	}
}
void PlaytimeRegion::OnTriggerExit(physics::Collider* collider)
{
	// 조건 더 다듬을 것
	if (Unit* colliderUnitComponent = collider->GetGameObject()->GetComponent<Unit>();
		colliderUnitComponent->GetUnitSide() == Unit::UnitSide::Player)
	{
		for (auto& each : OnLeave)
		{
			each();
		}
		// 가려져 있던 장식물들을 다시 보이게 하는 부분
	}
}
