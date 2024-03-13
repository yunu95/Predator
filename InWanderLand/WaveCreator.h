#pragma once
#include "YunutyEngine.h"

/// <summary>
/// UnitData.cpp에서 받은 waveData들을 받아서 wave를 만들어주는 컴포넌트.
/// region을 받아 배치 및 collider 생성.
/// 해당 region에 들어갈 유닛들 설정.
/// </summary>

class UnitProductor;
class WaveSystemComponent;

class WaveCreator : public Component, public SingletonComponent<WaveCreator>
{
private:
	GameObject* m_waveObject;
	WaveSystemComponent* m_waveSystemComponent;

public:
	void CreateNewRegion(Vector3d pos, float p_width, float p_height);
	void PushWaveUnitData(UnitProductor* p_fac, Vector3d p_pos, float p_delay);
	void SetWaveSizeVector(std::vector<int> p_vec);
};

