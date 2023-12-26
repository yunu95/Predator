#include "MeleeEnemyProduction.h"

void MeleeEnemyProduction::SetUnitData(GameObject* fbxObject)
{
	m_objectName = "MeleeEnemy";
	m_unitType = Unit::UnitType::MeleeEnemy;
	m_unitSide = Unit::UnitSide::Enemy;
	m_hp = 100;
	m_ap = 10;
	m_idRadius = 10.0f;
	m_atkRadius = 3.5f;
	m_unitSpeed = 1.5f;

	auto rsrcManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();

	/// Setting Animation Related Members
	m_unitGameObject = fbxObject;

	auto animator = m_unitGameObject->GetComponent<yunutyEngine::graphics::Animator>();

	auto& animList = rsrcManager->GetAnimationList();

	for (auto each : animList)
	{
		if (each->GetName() == L"Armature|idle")
		{
			m_idleAnimation = each;
			m_idleAnimation->SetLoop(true);
			animator->GetGI().PushAnimation(m_idleAnimation);
			animator->GetGI().Play(m_idleAnimation);
		}
		else if (each->GetName() == L"Armature|running")
		{
			m_walkAnimation = each;
			m_walkAnimation->SetLoop(true);
			animator->GetGI().PushAnimation(m_walkAnimation);
		}
		else if (each->GetName() == L"Armature|attack")
		{
			m_attackAnimation = each;
			m_attackAnimation->SetLoop(true);
			animator->GetGI().PushAnimation(m_attackAnimation);
		}
		else if (each->GetName() == L"Armature|Phase3_Player")
		{
			m_deathAnimation = each;
			m_deathAnimation->SetLoop(false);
			animator->GetGI().PushAnimation(m_deathAnimation);
		}
	}
}

yunutyEngine::GameObject* MeleeEnemyProduction::CreateUnitWithOrder()
{
	auto unitGameObject = UnitProductionOrder::CreateUnitWithOrder();
	return unitGameObject;
}
