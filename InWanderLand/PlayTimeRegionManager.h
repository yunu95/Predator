#pragma once
#include "YunutyEngine.h"
#include "PermanentObservee.h"

namespace application::editor
{
	class RegionData;
	class OrnamentData;
}
class Dotween;

/// <summary>
/// region간의 상관관계를 성립시켜주는 클래스.
/// </summary>
class PlayTimeRegionManager : public SingletonComponent<PlayTimeRegionManager>, public PermanentObservee
{
private:
	Vector3d stage2StartPosition;
	std::function<void()> stage1ToStage2Function;
	
	Dotween* cameraDotween;

	std::vector<GameObject*> stage1Ornaments;
	std::vector<GameObject*> stage2Ornaments;

public:
	virtual void Start() override;
	virtual void OnContentsStop() override;

	void AddRegionData(application::editor::RegionData* p_data);
	void RegisterOrnament(GameObject* p_obj, int p_stageNum);
	void SetCameraDotween(Dotween* p_dot);

	friend PlaytimeRegion;
};

