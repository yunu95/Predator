#include "WaveSystemComponent.h"
#include "UnitProductor.h"
#include "Unit.h"

void WaveSystemComponent::PushUnitData(UnitProductor* p_fac, Vector3d p_pos, float p_delay)
{
	m_waveDataMap.insert({ p_fac, {p_pos, p_delay} });
}

void WaveSystemComponent::SetWaveSizeInfo(std::vector<int> p_vec)
{
	m_waveSizeVector = p_vec;
}

void WaveSystemComponent::ProceedNextWave()
{
	isAllCurrentWaveUnitDead = true;
	m_deathCheckVector.clear();
	m_elapsed = 0.0f;
	isWaveStarted = true;

	if (m_waveSizeVector.size() == currentIndexCount)
	{
		/// wave 종료. 
	}
	else
	{
		currentIndexCount++;
	}
}

void WaveSystemComponent::Start()
{
	auto itr = m_waveDataMap.begin();

	/// 1. wave 갯수만큼 vector에 multimap을 push_back 해준다.
	for (auto& each : m_waveSizeVector)
	{
		std::multimap< UnitProductor*, std::pair<Vector3d, float> > tempMap;
		m_classifiedVector.push_back(tempMap);
	}

	/// 2. waveSizeVector[e] 는 해당 index(웨이브)에 생성될 유닛의 갯수를 의미한다.
	/// 그 수만큼 waveUnitData를 넣어주자
	
	int mapVectorIndex = 0;

	for (int e = 0; e < m_waveSizeVector.size(); e++)
	{
		for (int i = 0; i < m_waveSizeVector[e]; i++)
		{
			/// m_waveSizeVector[e] 만큼 m_waveDataMap의 앞쪽 데이터를 빼준다
			auto itr = m_waveDataMap.begin();
			m_classifiedVector[e].insert(*itr);
			m_waveDataMap.erase(itr);
		}
	}
}

void WaveSystemComponent::Update()
{
	/// Start() 에서 분류된 m_classifiedVector를 통해 유닛 생성
	if (isWaveStarted)
	{
		m_elapsed += Time::GetDeltaTime();

		if (m_classifiedVector[currentIndexCount].empty())
		{
			isWaveStarted = false;
		}

		for (auto itr = m_classifiedVector[currentIndexCount].begin(); itr != m_classifiedVector[currentIndexCount].end();)
		{
			if (m_elapsed >= itr->second.second)
			{
				GameObject* temp = itr->first->CreateUnit(itr->second.first);
				m_deathCheckVector.push_back(temp->GetComponent<Unit>());
				itr = m_classifiedVector[currentIndexCount].erase(itr);
			}
			else
			{
				itr++;
			}
		}
	}

	if (!m_deathCheckVector.empty())
	{
		for (auto& e : m_deathCheckVector)
		{
			if (e->GetCurrentUnitState() != Unit::UnitState::Death)
			{
				isAllCurrentWaveUnitDead = false;
				break;
			}
			else
			{
				isAllCurrentWaveUnitDead = true;
			}
		}
		if (isAllCurrentWaveUnitDead)
		{
			ProceedNextWave();
		}
	}

	if (yunutyEngine::Input::isKeyPushed(KeyCode::NUM_0))
	{
		for (auto& e : m_deathCheckVector)
		{
			e->SetUnitStateToDeath();
		}
	}
}

void WaveSystemComponent::OnTriggerEnter(physics::Collider* other)
{
	if (Unit* colliderUnitComponent = other->GetGameObject()->GetComponent<Unit>();
		colliderUnitComponent != nullptr &&
		colliderUnitComponent->GetUnitSide() == Unit::UnitSide::Player);
	{
		isWaveStarted = true;
	}
}
