#include "Trigger_EnterRegion.h"
#include "RegionData.h"
#include "PlaytimeRegion.h"

#include "Application.h"

namespace application
{
	void Trigger_EnterRegion::LinkCallback()
	{
		// Trigger_EnterRegion은 특정 지역에 플레이어 캐릭터가 진입할 시 호출됩니다.
		// LinkCallback 함수가 호출될 때 regionData는 이미 playtimeRegion을 생성한 상태여야 합니다.
		assert(enteringRegion && enteringRegion->playtimeRegion);
		enteringRegion->playtimeRegion->OnEnter.push_back([=]() { PullTrigger(); });
		// 지역 바깥으로 나갈 경우에는 아래와 같이 구현합니다.
		//enteringRegion->playtimeRegion->OnLeave.push_back([=]() {PullTrigger(); });
	}

	bool Trigger_EnterRegion::PreEncoding(json& data) const
	{
		return true;
	}

	bool Trigger_EnterRegion::PostEncoding(json& data) const
	{
		return true;
	}

	bool Trigger_EnterRegion::PreDecoding(const json& data)
	{
		return true;
	}

	bool Trigger_EnterRegion::PostDecoding(const json& data)
	{
		return true;
	}
}