#pragma once
#include "YunutyEngine.h"
#include <string>
#include <map>

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
class PlaytimeWave : public Component
{
private:
	int currentSequenceIndex{ 0 };
	int nextSummonUnitIndex{ 0 };

	bool isWaveActivated = false;

	float m_elapsed = 0.0f;

public:
	virtual ~PlaytimeWave();

	void ActivateWave();
	void DeActivateWave();

	virtual void Start() override;
	virtual void Update() override;
	application::editor::WaveData* waveData{ nullptr };
};