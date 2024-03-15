#include "PlaytimeWave.h"
#include "UnitData.h"
#include "WaveData.h"
#include "UnitProductor.h"


PlaytimeWave::~PlaytimeWave()
{
	waveData->playtimeWave = nullptr;
}

void PlaytimeWave::ActivateWave()
{
	isWaveActivated = true;
}
void PlaytimeWave::DeActivateWave()
{

}

void PlaytimeWave::Start()
{

}

void PlaytimeWave::Update()
{
	if (isWaveActivated)
	{
		m_elapsed += Time::GetDeltaTime();
	}
	auto& waveDelays = waveData->pod.waveDelays;
	auto waveSize = waveData->pod.waveSizes[currentSequenceIndex];
	auto sequenceSize = waveData->pod.waveSizes.size();
	// waveDelays는 delay가 오름차순으로 정렬되어 있다.
	// 현재 웨이브에서 아직 소환할 유닛이 더 남았는가?
	if (nextSummonUnitIndex < waveSize)
	{
		// 유닛 소환이 임박했는가?
		while (nextSummonUnitIndex < waveSize && waveDelays[nextSummonUnitIndex] < m_elapsed)
		{
			// 유닛을 소환하고 인덱스를 증가시킨다.
			// 유닛 데이터는 아래 값을 사용하면 됨.
			waveData->waveUnitDatasVector[nextSummonUnitIndex];
			Vector3d pos = { waveData->waveUnitDatasVector[nextSummonUnitIndex]->pod.position.x,
				waveData->waveUnitDatasVector[nextSummonUnitIndex]->pod.position.y,
				waveData->waveUnitDatasVector[nextSummonUnitIndex]->pod.position.z };
			// 유닛 소환 코드 ....
			m_productorVector[nextSummonUnitIndex]->CreateUnit(pos);
			nextSummonUnitIndex++;
		}
	}
	// 현재 웨이브에서 소환 대상이 되는 유닛들이 다 소환된 경우
	else
	{
		// 현재 등장한 유닛들이 다 죽었는가?
		if (false/*이 부분 바꿀것*/)
		{
			currentSequenceIndex++;
			// 웨이브에서 더 진행할 시퀀스가 없다면 웨이브를 중지하라.
			if (currentSequenceIndex >= sequenceSize)
			{
				DeActivateWave();
			}

		}
	}
}
