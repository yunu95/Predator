#include "Trigger_LeaveRegion.h"
#include "RegionData.h"
#include "PlaytimeRegion.h"

#include "Application.h"

namespace application
{
	void Trigger_LeaveRegion::LinkCallback()
	{
		// Trigger_LeaveRegion은 특정 지역에 플레이어 캐릭터가 이탈할 시 호출됩니다.
		// LinkCallback 함수가 호출될 때 regionData는 이미 playtimeRegion을 생성한 상태여야 합니다.
		assert(enteringRegion && enteringRegion->playtimeRegion);
		enteringRegion->playtimeRegion->OnLeave.push_back([=]() { PullTrigger(); });
	}

	void Trigger_LeaveRegion::SetRegion(editor::RegionData* region)
	{
		enteringRegion = region;
	}

	void Trigger_LeaveRegion::ImGui_DrawDataPopup(Trigger_LeaveRegion* data)
	{

	}

	bool Trigger_LeaveRegion::PreEncoding(json& data) const
	{
		return true;
	}

	bool Trigger_LeaveRegion::PostEncoding(json& data) const
	{
		return true;
	}

	bool Trigger_LeaveRegion::PreDecoding(const json& data)
	{
		return true;
	}

	bool Trigger_LeaveRegion::PostDecoding(const json& data)
	{
		return true;
	}
}