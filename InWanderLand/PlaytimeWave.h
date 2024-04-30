#pragma once
#include "YunutyEngine.h"
#include "Unit.h"
#include <string>
#include <map>
#include "ContentsObservee.h"

namespace application
{
	namespace editor
	{
		class POD_Unit;
		class WaveData;
	}
}

class UnitProductor;

/// <summary>
/// 플레이타임에서 웨이브 하나에 대응되는 컴포넌트
/// </summary>
class PlaytimeWave : public Component, public ContentsObservee
{
private:
	int currentSequenceIndex{ 0 };
	int nextSummonUnitIndex{ 0 };
	int waveDataIndex{ 0 };

	bool isWaveActivated = false;

	float m_elapsed{ 0.0f };

	std::vector<UnitProductor*> productorSelector;

public:
	std::vector<Unit*> m_currentWaveUnitVector;
	Unit* inGameUnit;
	virtual ~PlaytimeWave();

	virtual void PlayFunction() override;
	virtual void StopFunction() override;

	void ActivateWave();
	void DeActivateWave();

	virtual void Start() override;
	virtual void Update() override;
	application::editor::WaveData* waveData{ nullptr };
};