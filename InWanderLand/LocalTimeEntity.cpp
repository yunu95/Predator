#include "LocalTimeEntity.h"

LocalTimeEntity::LocalTimeEntity()
{
	LocalTimeEntityManager::Instance().RegisterLocalTimeEntity(this);
}

void LocalTimeEntity::SetLocalTimeScale(float p_timeScale)
{
	m_localTimeScale = p_timeScale;
}
