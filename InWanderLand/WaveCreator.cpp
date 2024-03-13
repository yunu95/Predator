#include "WaveCreator.h"
#include "UnitProductor.h"
#include "WaveSystemComponent.h"


void WaveCreator::CreateNewRegion(Vector3d pos, float p_width, float p_height)
{
	m_waveObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	auto regionColliderComponent = m_waveObject->AddComponent<physics::BoxCollider>();
	regionColliderComponent->SetHalfExtent({ p_width , 0.1f, p_height });
	m_waveObject->GetTransform()->SetWorldPosition(pos);
	m_waveSystemComponent = m_waveObject->AddComponent<WaveSystemComponent>();
}

void WaveCreator::PushWaveUnitData(UnitProductor* p_fac, Vector3d p_pos, float p_delay)
{
	m_waveSystemComponent->PushUnitData(p_fac, p_pos, p_delay);
}

void WaveCreator::SetWaveSizeVector(std::vector<int> p_vec)
{
	m_waveSystemComponent->SetWaveSizeInfo(p_vec);
}
