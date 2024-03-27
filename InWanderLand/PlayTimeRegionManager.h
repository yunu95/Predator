#pragma once
#include "YunutyEngine.h"

namespace application::editor
{
	class RegionData;
}
class Dotween;

/// <summary>
/// region간의 상관관계를 성립시켜주는 클래스.
/// </summary>
class PlayTimeRegionManager : public Component, public SingletonComponent<PlayTimeRegionManager>
{
private:
	Vector3d stage2StartPosition;
	std::function<void()> stage1ToStage2Function;
	
	Dotween* cameraDotween;

public:
	void AddRegionData(application::editor::RegionData* p_data);
	void SetCameraDotween(Dotween* p_dot);
	friend PlaytimeRegion;
};

