#include "LocalTimeEntityManager.h"
#include "LocalTimeEntity.h"

LocalTimeEntityManager::~LocalTimeEntityManager()
{
	int a = 0;
}

void LocalTimeEntityManager::Start()
{
}

void LocalTimeEntityManager::OnContentsStop()
{
	this->SetActive(false);
	m_entitySet.clear();
}

void LocalTimeEntityManager::RegisterLocalTimeEntity(LocalTimeEntity* p_entity)
{
	m_entitySet.insert(p_entity);
}

void LocalTimeEntityManager::ReportTacticModeEngaged()
{
	for (auto each : m_entitySet)
	{
		each->SetLocalTimeScale(0.0f);
	}
}

void LocalTimeEntityManager::ReportTacticModeEnded()
{
	for (auto each : m_entitySet)
	{
		each->SetLocalTimeScale(1.0f);
	}
}

void LocalTimeEntityManager::SetLocalTimeScaleDirectly(LocalTimeEntity* p_entity, float p_scale)
{
	p_entity->SetLocalTimeScale(p_scale);
}
