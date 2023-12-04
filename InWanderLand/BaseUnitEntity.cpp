#include "BaseUnitEntity.h"
#include "Dotween.h"

std::string BaseUnitEntity::GetType() const
{
	return unitType;
}

void BaseUnitEntity::SetIdRadius(float radius)
{
	m_IdDistance = radius;
}

void BaseUnitEntity::SetAtkRadius(float radius)
{
	m_AtkDistance = radius;
}

/// 상대 유닛이 인식 범위를 완전히 벗어낫을 때 호출되는 함수.
/// ex) FSM 초기화, transitionDelay 초기화.
void BaseUnitEntity::InitFSM()
{
	transitionDelay = 0.0f;
	initToIdle = true;
}

void BaseUnitEntity::SetOpponentGameObject(yunutyEngine::GameObject* obj)
{
	m_opponentGameObjectList.push_back(obj);
	if (m_currentTargetObject == nullptr)
		m_currentTargetObject = obj;
}

void BaseUnitEntity::DeleteOpponentGameObject(yunutyEngine::GameObject* obj)
{
	m_opponentGameObjectList.remove(obj);
}

void BaseUnitEntity::EnterIDRange()
{
	isJustEntered = true;
}
