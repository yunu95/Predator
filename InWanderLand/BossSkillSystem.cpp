#include "BossSkillSystem.h"
#include "PlayerController.h"
#include "Unit.h"
#include "EnemySummonGateProductor.h"
#include "EnemySummonSkillSystem.h"
#include "ShortcutSystem.h"

void BossSkillSystem::ActivateSkill(Unit::SkillEnum p_currentSkill)
{
	/// 스킬 종류에 따라 스킬 사용 위치를 정해주자.
	switch (p_currentSkill)
	{
		case Unit::SkillEnum::BossSkillOne:
			ActivateSkillOne();
			break;
		case Unit::SkillEnum::BossSkillTwo:
			ActivateSkillTwo();
			break;
		case Unit::SkillEnum::BossSkillThree:
			ActivateSkillThree();
			break;
		case Unit::SkillEnum::BossSkillFour:
			ActivateSkillFour();
			break;
		default:
			break;
	}
}

void BossSkillSystem::ActivateSkillOne()
{
	SetSkillRequirmentsActive(m_skillOneRequirments, true);
	m_unitComponent->SetSkillDuration(m_skillOneDuration);
	m_unitDotween->DONothing(m_skillOneDuration).OnComplete([=]()
		{
			SetSkillRequirmentsActive(m_skillOneRequirments, false);
		});
}

void BossSkillSystem::ActivateSkillTwo()
{
	SetSkillRequirmentsActive(m_skillTwoRequirments, true);
	m_unitComponent->SetSkillDuration(m_skillTwoDuration);
	m_unitDotween->DONothing(m_skillTwoDuration).OnComplete([=]()
		{
			SetSkillRequirmentsActive(m_skillTwoRequirments, false);
		});
}

void BossSkillSystem::ActivateSkillThree()
{
	///// 유닛을 생성하는 문 오브젝트를 생성하는 로직
	//Vector3d doorSummonPosition = GetTransform()->GetWorldPosition() + GetTransform()->GetWorldRotation().Forward() * -5.0f;
	////UnitObjectPool::SingleInstance().ChooseProductor(&EnemySummonGateProductor::Instance());
	////UnitObjectPool::SingleInstance().SetStartPosition(doorSummonPosition);
	//EnemySummonGateProductor::Instance().SetUnitCanBeDamaged(true);
	//currentSummonedDoorUnit = UnitObjectPool::SingleInstance().Borrow()->m_pairUnit;
	//application::ShortcutSystem::Instance().RegisterTriggerFunction(2,
	//	[=]() { currentSummonedDoorUnit->GetGameObject()->SetSelfActive(!currentSummonedDoorUnit->GetGameObject()->GetSelfActive()); });


	//if (currentDerivedDoorUnit == nullptr)
	//{
	//	doorSummonPosition = GetTransform()->GetWorldPosition() + GetTransform()->GetWorldRotation().Forward() * 5.0f;
	//	UnitObjectPool::SingleInstance().ChooseProductor(&EnemySummonGateProductor::Instance());
	//	UnitObjectPool::SingleInstance().SetStartPosition(doorSummonPosition);
	//	EnemySummonGateProductor::Instance().SetUnitCanBeDamaged(false);
	//	currentDerivedDoorUnit = UnitObjectPool::SingleInstance().Borrow()->m_pairUnit;
	//	application::ShortcutSystem::Instance().RegisterTriggerFunction(2,
	//		[=]() { currentDerivedDoorUnit->GetGameObject()->SetSelfActive(!currentDerivedDoorUnit->GetGameObject()->GetSelfActive()); });
	//}
	//else
	//{
	//	doorSummonPosition = GetTransform()->GetWorldPosition() + GetTransform()->GetWorldRotation().Forward() * 5.0f;
	//	currentDerivedDoorUnit->GetTransform()->SetWorldPosition(doorSummonPosition);
	//	currentDerivedDoorUnit->GetGameObject()->SetSelfActive(true);
	//}

}

void BossSkillSystem::ActivateSkillFour()
{
	int maxXZpos = 20;
	int minXZpos = 5;

	int tempXpos = rand() % (maxXZpos + minXZpos) + minXZpos;
	int tempZpos = rand() % (maxXZpos + minXZpos) + minXZpos;

	float selectedXpos = static_cast<float>(tempXpos);
	float selectedZpos = static_cast<float>(tempZpos);

	m_skillFourRequirments.colliderObject->GetTransform()->SetWorldPosition({ selectedXpos, 0.0f, selectedZpos });
	m_skillFourRequirments.debugObject->GetTransform()->SetWorldPosition({ selectedXpos, 0.0f, selectedZpos });

	SetSkillRequirmentsActive(m_skillFourRequirments, true);

	m_unitComponent->SetSkillDuration(m_skillFourDuration);

	m_unitDotween->DONothing(m_skillFourDuration).OnComplete([=]()
		{
			SetSkillRequirmentsActive(m_skillFourRequirments, false);
		});
}

void BossSkillSystem::ActivateSkillRandomly()
{
	m_skillNum = rand() % 4 + 2;

	if (m_skillNum == static_cast<int>(Unit::SkillEnum::BossSkillThree) && currentSummonedDoorUnit != nullptr && currentSummonedDoorUnit->GetCurrentUnitState() != Unit::UnitState::Death)
	{
		ActivateSkillRandomly();
	}
	else
	{
		//m_skillNum = 3;
		ActivateSkill(static_cast<Unit::SkillEnum>(m_skillNum));
	}
}

void BossSkillSystem::SetSkillOneRequirments(GameObject* p_obj, physics::SphereCollider* p_projectileCollider, GameObject* p_debugObj)
{
	m_skillOneRequirments.colliderObject = p_obj;
	m_skillOneRequirments.skillCollider = p_projectileCollider;
	m_skillOneRequirments.debugObject = p_debugObj;
}

void BossSkillSystem::SetSkillTwoRequirments(GameObject* p_obj, physics::BoxCollider* p_projectileCollider, GameObject* p_debugObj)
{
	m_skillTwoRequirments.colliderObject = p_obj;
	m_skillTwoRequirments.skillCollider = p_projectileCollider;
	m_skillTwoRequirments.debugObject = p_debugObj;
}

void BossSkillSystem::SetSkillTwoRange(float p_range)
{
	m_skillTwoColliderRange = p_range;
}

void BossSkillSystem::SetSkillFourRequirments(GameObject* p_obj, physics::SphereCollider* p_projectileCollider, GameObject* p_debugObj)
{
	m_skillFourRequirments.colliderObject = p_obj;
	m_skillFourRequirments.skillCollider = p_projectileCollider;
	m_skillFourRequirments.debugObject = p_debugObj;
}

void BossSkillSystem::Start()
{
	m_skillUsageDuration = 10.0f;
	m_warriorUnit = PlayerController::SingleInstance().GetPlayerMap().find(Unit::UnitType::Warrior)->second;
	SetOtherComponentsAsMember();

	m_skillOneRequirments.colliderObject->SetParent(GetGameObject());
	m_skillOneRequirments.debugObject->SetParent(GetGameObject());
	m_skillTwoRequirments.colliderObject->SetParent(GetGameObject());
	m_skillTwoRequirments.debugObject->SetParent(GetGameObject());
	m_skillFourRequirments.colliderObject->SetParent(GetGameObject());
	m_skillFourRequirments.debugObject->SetParent(GetGameObject());

	m_skillTwoRequirments.colliderObject->GetTransform()->SetLocalPosition({ 0.0f, 0.0f, m_skillTwoColliderRange * -1 });
	m_skillTwoRequirments.debugObject->GetTransform()->SetLocalPosition({ 0.0f, 0.0f, m_skillTwoColliderRange * -1 });

	SetSkillRequirmentsActive(m_skillOneRequirments, false);
	SetSkillRequirmentsActive(m_skillTwoRequirments, false);
	SetSkillRequirmentsActive(m_skillFourRequirments, false);

	//m_magicianUnit = PlayerController::SingleInstance().GetPlayerMap().find(Unit::UnitType::Magician)->second;
	//m_healerUnit = PlayerController::SingleInstance().GetPlayerMap().find(Unit::UnitType::Healer)->second;
}

void BossSkillSystem::Update()
{
	m_elapsed += Time::GetDeltaTime();

	if (m_elapsed >= m_skillUsageDuration)
	{
		m_unitComponent->SetUnitStateToSkill();
		m_elapsed = 0.0f;
	}


	if (currentSummonedDoorUnit != nullptr && currentSummonedDoorUnit->GetCurrentUnitState() == Unit::UnitState::Death)
	{
		currentSummonedDoorUnit = nullptr;
		currentDerivedDoorUnit->GetGameObject()->SetSelfActive(false);
	}
}

