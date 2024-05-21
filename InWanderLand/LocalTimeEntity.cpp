#include "LocalTimeEntity.h"

LocalTimeEntity::LocalTimeEntity()
{
    LocalTimeEntityManager::Instance().RegisterLocalTimeEntity(this);
}

LocalTimeEntity::~LocalTimeEntity()
{
    LocalTimeEntityManager::Instance().m_entitySet.erase(this);
}

void LocalTimeEntity::SetLocalTimeScale(float p_timeScale)
{
    m_localTimeScale = p_timeScale;
}
