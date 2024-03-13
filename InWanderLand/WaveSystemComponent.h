#pragma once
#include "YunutyEngine.h"

/// <summary>
/// TimeBase / DefeatBased WaveSystem 등 모든 WaveSystem의 부모 클래스.
/// </summary>

class UnitProductor;
class Unit;

class WaveSystemComponent : public Component
{
private:
	std::multimap< UnitProductor*, std::pair<Vector3d, float> > m_waveDataMap;
	std::vector<std::multimap< UnitProductor*, std::pair<Vector3d, float> > > m_classifiedVector;
	bool isWaveStarted = true;
	float m_elapsed = 0.0f;

	std::vector<int> m_waveSizeVector;
	int waveSizeIndex = 0;
	int currentIndexCount = 0;

	std::vector<Unit*> m_deathCheckVector;
	bool isAllCurrentWaveUnitDead = true;

public:
	void PushUnitData(UnitProductor* p_fac, Vector3d p_pos, float p_delay);
	void SetWaveSizeInfo(std::vector<int> p_vec);

	void ProceedNextWave();

public:
	virtual void Start() override;
	virtual void Update() override;
	virtual void OnTriggerEnter(physics::Collider* other) override;
};

