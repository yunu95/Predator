#include "InWanderLand.h"
#include "Unit.h"
#include "PlayerController.h"
#include "AttackSystem.h"
#include "RangedAttackSystem.h"
#include "InputManager.h"
#include "PlayerSkillSystem.h"
#include "ChessTrapSkillSystem.h"
#include "SpikeSkillSystem.h"
#include "BossSkillSystem.h"
#include "TacticModeSystem.h"
#include "IAnimation.h"
#include "VFXAnimator.h"
#include "SkillPreviewSystem.h"

#include "BurnEffect.h"
#include "DebuggingMeshPool.h"
#include "StatusEffect.h"
#include "CinematicManager.h"
#include "UnitBuff.h"
#include "wanderResources.h"
#include "FBXPool.h"
#include "ProjectileType.h"
#include "RTSCam.h"

coroutine::Coroutine ShowPath(const std::vector<Vector3d> paths)
{
	for (int i = 0; i < paths.size(); i++)
	{
		DebugBeacon::PlaceBeacon(paths[i], yunuGI::Color::green(), Vector3d::one * 0.5, 0.5);
	}
	co_return;
}
float getDeltaAngle(float difference) {
	while (difference < -180) difference += 360;
	while (difference > 180) difference -= 360;
	return difference;
}
bool Unit::pauseAll{ false };
Unit* Unit::debuggingUnit{ nullptr };

std::weak_ptr<Unit> Unit::GetClosestEnemy()
{
	if (auto unit = GetClosestEnemyWithinAttackRange().lock())
		return unit;

	if (auto unit = GetClosestEnemyWithinAcquisitionRange().lock())
		return unit;

	return std::weak_ptr<Unit>();
}
std::weak_ptr<Unit> Unit::GetClosestEnemyWithinAttackRange()
{
	auto minIt = std::min_element(attackRange.lock()->GetEnemies().begin(), attackRange.lock()->GetEnemies().end(), [this](const Unit* const a, const Unit* const b)
		{
			return (GetTransform()->GetWorldPosition() - a->GetTransform()->GetWorldPosition()).MagnitudeSqr() <
				(GetTransform()->GetWorldPosition() - b->GetTransform()->GetWorldPosition()).MagnitudeSqr();
		});

	if (minIt != attackRange.lock()->GetEnemies().end())
	{
		auto tempPrimitivePtr = *minIt;
		int a = 3;
	}
	if (minIt != attackRange.lock()->GetEnemies().end())
		return (*minIt)->GetWeakPtr<Unit>();
	return std::weak_ptr<Unit>();
}
std::weak_ptr<Unit> Unit::GetClosestEnemyWithinAcquisitionRange()
{
	auto minIt = std::min_element(acquisitionRange.lock()->GetEnemies().begin(), acquisitionRange.lock()->GetEnemies().end(), [this](const Unit* const a, const Unit* const b)
		{
			return (GetTransform()->GetWorldPosition() - a->GetTransform()->GetWorldPosition()).MagnitudeSqr() <
				(GetTransform()->GetWorldPosition() - b->GetTransform()->GetWorldPosition()).MagnitudeSqr();
		});

	if (minIt != acquisitionRange.lock()->GetEnemies().end())
	{
		auto tempPrimitivePtr = *minIt;
		int a = 3;
	}
	if (minIt != acquisitionRange.lock()->GetEnemies().end())
		return (*minIt)->GetWeakPtr<Unit>();

	return std::weak_ptr<Unit>();
}
void Unit::Revive()
{
	if (!IsAlive() && coroutineRevival.expired())
	{
		StartCoroutine(RevivalCoroutine(0));
	}
}
void Unit::Update()
{
	if (referenceDisableNavAgent.BeingReferenced())
	{
		navAgentEnableFrameCount = 0;
		navAgentComponent.lock()->SetActive(false);
	}
	else
	{
		// navAgent 복구는 한 프레임 이후에 진행된다.
		if (navAgentEnableFrameCount >= 1)
			navAgentComponent.lock()->SetActive(!referenceDisableNavAgent.BeingReferenced());
		navAgentEnableFrameCount++;
	}
	navObstacle.lock()->SetActive(referenceEnableNavObstacle.BeingReferenced());
	//navObstacle.lock()->SetActive(false);

	if (!referenceBlockFollowingNavAgent.BeingReferenced())
		GetTransform()->SetWorldPosition(navAgentComponent.lock()->GetTransform()->GetWorldPosition());
	if (!referenceBlockRotation.BeingReferenced())
		UpdateRotation();
	// UI가 유닛을 따라다니게 만듬
	auto& offset = unitTemplateData->pod.statusBarOffset;
	for (auto unitStatusUI : unitStatusUIs)
	{
		if (!unitStatusUI.expired() && (unitStatusUI.lock()->runtimeFlags & UnitStatusBarFlag::FollowUnit))
		{
			unitStatusUI.lock()->GetTransform()->SetWorldPosition(Vector3d{ offset.x,offset.y,0 }
			+ UIManager::Instance().GetUIPosFromWorld(GetTransform()->GetWorldPosition()));
		}
	}
	// 버프 효과 적용
	for (auto& [buffID, buff] : buffs)
	{
		buff.get()->OnUpdate();
		buff.get()->durationLeft -= Time::GetDeltaTime() * localBuffTimeScale;
		if (buff.get()->durationLeft < 0 || currentHitPoint <= 0.0f)
			buff.get()->OnEnd();
	}
	std::erase_if(buffs, [](std::pair<const UnitBuffType, std::shared_ptr<UnitBuff>>& pair)
		{ return pair.second->durationLeft < 0; });
	// behaviour tree에 맞게 동작
	unitBehaviourTree.Update();
	auto navPos = navAgentComponent.lock()->GetTransform()->GetWorldPosition();
	auto navSpeed = navAgentComponent.lock()->GetSpeed();
	lastPosition = GetTransform()->GetWorldPosition();
	// 재생중인 애니메이션이 없다면 기본 애니메이션 출력
	// 어떤게 기본 애니메이션인지는 행동트리의 상태에 따라 바뀔 수 있다.
	if (!referenceBlockAnimLoop.BeingReferenced() && (animatorComponent.lock()->IsDone() || blendWithDefaultAnimTrigger) && defaultAnimationType != UnitAnimType::None)
	{
		blendWithDefaultAnimTrigger = false;
		PlayAnimation(defaultAnimationType);
	}
	// 아웃라인 출력여부를 판별한다.
	if (referenceSelectOutline.BeingReferenced())
	{
		skinnedMeshRenderer->GetGI().SetOutLineInfo(true, yunuGI::Color::green());
	}
	else
	{
		if (referenceHoverOutline.BeingReferenced())
		{
			skinnedMeshRenderer->GetGI().SetOutLineInfo(true, teamIndex == PlayerController::playerTeamIndex ? yunuGI::Color::white() : yunuGI::Color::red());
		}
		else
		{
			skinnedMeshRenderer->GetGI().SetOutLineInfo(false, yunuGI::Color::white());
		}
	}
	Reference referenceSelectOutline;
}

void Unit::OnDestroy()
{
	for (auto unitStatusUI : unitStatusUIs)
	{
		if (!unitStatusUI.expired() && (unitStatusUI.lock()->runtimeFlags & UnitStatusBarFlag::DestroyOnDeath))
			Scene::getCurrentScene()->DestroyGameObject(unitStatusUI.lock()->GetGameObject());
	}
	if (!navAgentComponent.expired())
		Scene::getCurrentScene()->DestroyGameObject(navAgentComponent.lock()->GetGameObject());
}

template<>
void Unit::OnStateEngage<UnitBehaviourTree::Death>()
{
	onStateEngage[UnitBehaviourTree::Death]();
	PlayAnimation(UnitAnimType::Death);
	enableNavObstacleByState.reset();
	disableNavAgentByState = referenceDisableNavAgent.Acquire();
	defaultAnimationType = UnitAnimType::None;
	if (belongingWave)
		belongingWave->ReportUnitDeath(this);
	coroutineDeath = StartCoroutine(DeathCoroutine());
	for (auto& [buffID, buff] : buffs)
	{
		buff.get()->OnEnd();
	}
}
template<>
void Unit::OnStateExit<UnitBehaviourTree::Death>()
{
	onStateExit[UnitBehaviourTree::Death]();
	disableNavAgentByState.reset();
}
template<>
void Unit::OnStateEngage<UnitBehaviourTree::Paralysis>()
{
	onStateEngage[UnitBehaviourTree::Paralysis]();
}
template<>
void Unit::OnStateExit<UnitBehaviourTree::Paralysis>()
{
	onStateExit[UnitBehaviourTree::Paralysis]();
}
template<>
void Unit::OnStateEngage<UnitBehaviourTree::Knockback>()
{
	onStateEngage[UnitBehaviourTree::Knockback]();
	blockFollowingNavAgentByState = referenceBlockFollowingNavAgent.Acquire();
	PlayAnimation(UnitAnimType::Airborne, Animation::PlayFlag_::Blending | Animation::PlayFlag_::Repeat);
}
template<>
void Unit::OnStateEngage<UnitBehaviourTree::Stun>()
{
	onStateEngage[UnitBehaviourTree::Knockback]();
	PlayAnimation(UnitAnimType::Paralysis, Animation::PlayFlag_::Blending | Animation::PlayFlag_::Repeat);
}
template<>
void Unit::OnStateUpdate<UnitBehaviourTree::Knockback>()
{
}
template<>
void Unit::OnStateExit<UnitBehaviourTree::Knockback>()
{
	onStateExit[UnitBehaviourTree::Knockback]();
	blockFollowingNavAgentByState.reset();
	OrderAttackMove();
}
template<>
void Unit::OnStateEngage<UnitBehaviourTree::Pause>()
{
	onStateEngage[UnitBehaviourTree::Pause]();
	defaultAnimationType = UnitAnimType::Idle;
	enableNavObstacleByState = referenceEnableNavObstacle.Acquire();
	disableNavAgentByState = referenceDisableNavAgent.Acquire();
}
template<>
void Unit::OnStateExit<UnitBehaviourTree::Pause>()
{
	onStateExit[UnitBehaviourTree::Pause]();
	enableNavObstacleByState.reset();
	disableNavAgentByState.reset();
}
template<>
void Unit::OnStateEngage<UnitBehaviourTree::Reviving>()
{
	onStateEngage[UnitBehaviourTree::Reviving]();
	enableNavObstacleByState = referenceEnableNavObstacle.Acquire();
	disableNavAgentByState = referenceDisableNavAgent.Acquire();
	coroutineRevival = StartCoroutine(RevivalCoroutine(unitTemplateData->pod.revivalDuration));
}
template<>
void Unit::OnStateExit<UnitBehaviourTree::Reviving>()
{
	onStateExit[UnitBehaviourTree::Reviving]();
	enableNavObstacleByState.reset();
	disableNavAgentByState.reset();
}
template<>
void Unit::OnStateUpdate<UnitBehaviourTree::Chasing>()
{
	if (!referenceBlockPendingOrder.BeingReferenced() && !pendingTargetUnit.expired())
	{
		currentTargetUnit = pendingTargetUnit;
		if (currentTargetUnit.lock().get() == this)
		{
			int a = 1;
		}
		pendingTargetUnit.reset();
	}
}
template<>
void Unit::OnStateEngage<UnitBehaviourTree::Attack>()
{
	onStateEngage[UnitBehaviourTree::Attack]();
	PlayAnimation(UnitAnimType::Idle, Animation::PlayFlag_::Blending | Animation::PlayFlag_::NonRedundant);
	defaultAnimationType = UnitAnimType::Idle;
	enableNavObstacleByState = referenceEnableNavObstacle.Acquire();
	disableNavAgentByState = referenceDisableNavAgent.Acquire();
}
template<>
void Unit::OnStateExit<UnitBehaviourTree::Attack>()
{
	DeleteCoroutine(coroutineAttack);
	DeleteCoroutine(coroutineAttackEffect);
	animatorComponent.lock()->GetGI().SetPlaySpeed(1);
	onStateExit[UnitBehaviourTree::Attack]();
	enableNavObstacleByState.reset();
	disableNavAgentByState.reset();
}
template<>
void Unit::OnStateUpdate<UnitBehaviourTree::Attack>()
{
	auto currentTarget = currentTargetUnit.lock();
	if (!currentTarget)
	{
		return;
	}
	else
	{
		if (!currentTarget->GetGameObject()->GetActive() || !currentTarget->IsAlive() || currentTarget->IsInvulenerable())
		{
			currentTargetUnit.reset();
			return;
		}
	}
	SetDesiredRotation(currentTargetUnit.lock()->GetTransform()->GetWorldPosition() - GetTransform()->GetWorldPosition());
	if (!referenceBlockAttack.BeingReferenced() && coroutineAttack.expired())
	{
		coroutineAttack = StartCoroutine(AttackCoroutine(currentTargetUnit));
		coroutineAttack.lock()->PushDestroyCallBack([this]()
			{
				if (!animatorComponent.expired())
				{
					animatorComponent.lock()->GetGI().SetPlaySpeed(1);
				}
			});
	}
}
template<>
void Unit::OnStateEngage<UnitBehaviourTree::Move>()
{
    onStateEngage[UnitBehaviourTree::Move]();
    auto debugtrs = navAgentComponent.lock()->GetTransform();
    navAgentComponent.lock()->SetSpeed(unitTemplateData->pod.m_unitSpeed);
    //StartCoroutine(ShowPath(SingleNavigationField::Instance().GetSmoothPath(GetTransform()->GetWorldPosition() + GetTransform()->GetWorldRotation().Forward() * unitTemplateData->pod.collisionSize, moveDestination)));
    PlayAnimation(UnitAnimType::Move, Animation::PlayFlag_::Blending | Animation::PlayFlag_::Repeat);
	yunuGI::IAnimation* anim = wanderResources::GetAnimation(unitTemplateData->pod.skinnedFBXName, UnitAnimType::Move);
	anim->SetPlaySpeed(unitTemplateData->pod.m_moveAnimationSpeed);
    jammedDuration = 0;
}
template<>
void Unit::OnStateExit<UnitBehaviourTree::Move>()
{
	onStateExit[UnitBehaviourTree::Move]();
}
template<>
void Unit::OnStateUpdate<UnitBehaviourTree::Move>()
{
	static constexpr float epsilon = 0.1f;
	auto currentPosition = GetTransform()->GetWorldPosition();
	/*if ((moveDestination - currentPosition).MagnitudeSqr() < epsilon)
	{
		OrderAttackMove(moveDestination);
	}*/
	static constexpr float jamFactor = 0.1f;
	float idealDeltaPosition = unitTemplateData->pod.m_unitSpeed * Time::GetDeltaTime();
	if ((lastPosition - currentPosition).MagnitudeSqr() < idealDeltaPosition * idealDeltaPosition * jamFactor)
	{
		PlayAnimation(UnitAnimType::Idle, Animation::PlayFlag_::Blending | Animation::PlayFlag_::Repeat | Animation::PlayFlag_::NonRedundant);
		if ((jammedDuration += Time::GetDeltaTime()) > jamDurationThreshold)
		{
			OrderAttackMove();
			jammedDuration = 0;
		}
		SetDesiredRotation(moveDestination - lastPosition);
	}
	else
	{
		PlayAnimation(UnitAnimType::Move, Animation::PlayFlag_::Blending | Animation::PlayFlag_::Repeat | Animation::PlayFlag_::NonRedundant);
		jammedDuration = 0;
		SetDesiredRotation(GetTransform()->GetWorldPosition() - lastPosition);
	}
	navAgentComponent.lock()->MoveTo(moveDestination);
}
template<>
void Unit::OnStateEngage<UnitBehaviourTree::Skill>()
{
	onStateEngage[UnitBehaviourTree::Skill]();
	currentOrderType = UnitOrderType::Skill;
}
template<>
void Unit::OnStateExit<UnitBehaviourTree::SkillOnGoing>()
{
	onStateExit[UnitBehaviourTree::SkillOnGoing]();
	onSkillExpiration(onGoingSkill);
	if (!coroutineSkill.expired())
	{
		onGoingSkill->OnInterruption();
		DeleteCoroutine(coroutineSkill);
	}
	onGoingSkill.reset();
	//if (pendingOrderType == UnitOrderType::None)
	//    OrderAttackMove();
}
template<>
void Unit::OnStateEngage<UnitBehaviourTree::SkillCasting>()
{
	assert(pendingSkill.get() != nullptr);
	SetDesiredRotation(pendingSkill.get()->targetPos - GetTransform()->GetWorldPosition());
	onGoingSkill = std::move(pendingSkill);
	coroutineSkill = StartCoroutine(onGoingSkill.get()->operator()());
	onSkillActivation(onGoingSkill);
}
template<>
void Unit::OnStateEngage<UnitBehaviourTree::Tactic>()
{
	onStateEngage[UnitBehaviourTree::Tactic]();
	PlayAnimation(UnitAnimType::Idle, Animation::PlayFlag_::Blending | Animation::PlayFlag_::Repeat);
	enableNavObstacleByState = referenceEnableNavObstacle.Acquire();
	disableNavAgentByState = referenceDisableNavAgent.Acquire();
}
template<>
void Unit::OnStateExit<UnitBehaviourTree::Tactic>()
{
	onStateExit[UnitBehaviourTree::Tactic]();
	enableNavObstacleByState.reset();
	disableNavAgentByState.reset();
}

template<>
void Unit::OnStateEngage<UnitBehaviourTree::Stop>()
{
	onStateEngage[UnitBehaviourTree::Stop]();
	PlayAnimation(UnitAnimType::Idle, Animation::PlayFlag_::Blending | Animation::PlayFlag_::Repeat | Animation::PlayFlag_::NonRedundant);
	enableNavObstacleByState = referenceEnableNavObstacle.Acquire();
	disableNavAgentByState = referenceDisableNavAgent.Acquire();
}
template<>
void Unit::OnStateExit<UnitBehaviourTree::Stop>()
{
	onStateExit[UnitBehaviourTree::Stop]();
	enableNavObstacleByState.reset();
	disableNavAgentByState.reset();
}

Unit::~Unit()
{
	for (auto unitStatusUI : unitStatusUIs)
	{
		if (!unitStatusUI.expired() && (unitStatusUI.lock()->runtimeFlags & UnitStatusBarFlag::DestroyOnDeath))
		{
			Scene::getCurrentScene()->DestroyGameObject(unitStatusUI.lock()->GetGameObject());
		}
	}
	if (!navAgentComponent.expired())
	{
		Scene::getCurrentScene()->DestroyGameObject(navAgentComponent.lock()->GetGameObject());
	}
}

void Unit::OnPause()
{
	isPaused = true;
	localBuffTimeScale = FLT_MIN * 10000;
	if (!IsPlayerUnit())
	{
		localTimeScale = FLT_MIN * 10000;
		animatorComponent.lock()->Pause();

		for (auto& each : GetGameObject()->GetChildren())
		{
			if (auto ptr = each->GetComponent<graphics::ParticleRenderer>())
			{
				ptr->Pause();
			}
		}
	}
}

void Unit::OnResume()
{
	isPaused = false;
	localBuffTimeScale = 1.0f;
	if (!IsPlayerUnit())
	{
		localTimeScale = 1.0f;
		animatorComponent.lock()->Resume();

		for (auto& each : GetGameObject()->GetChildren())
		{
			if (auto ptr = each->GetComponent<graphics::ParticleRenderer>())
			{
				ptr->Resume();
			}
		}
	}
}

bool Unit::IsPlayerUnit() const
{
	return unitTemplateData->pod.playerUnitType.enumValue != PlayerCharacterType::None;
}
bool Unit::IsInvulenerable() const
{
	return referenceInvulnerable.BeingReferenced();
}
bool Unit::IsAlive() const
{
	//return isAlive;
	return isAlive && GetGameObject()->GetSelfActive();
}

bool Unit::IsPreempted() const
{
	return (*unitBehaviourTree.GetActiveNodes().rbegin())->GetNodeKey() == UnitBehaviourTree::Knockback;
}

bool Unit::IsTacTicReady() const
{
	for (auto each : unitBehaviourTree.GetActiveNodes())
	{
		if (each->GetNodeKey() == UnitBehaviourTree::Tactic)
		{
			return true;
		}
	}

	return false;
}

float Unit::GetCritChance()
{
	return unitTemplateData->pod.m_critChance + adderCritChance;
}

float Unit::GetCritMultiplier()
{
	return unitTemplateData->pod.m_critMultiplier;
}

int Unit::GetArmor()
{
	return unitTemplateData->pod.m_armor;
}

float Unit::GetProjectileSpeed()
{
	return unitTemplateData->pod.projectileSpeed * multiplierProjectileSpeed;
}

float Unit::GetEvasionChance()
{
	return unitTemplateData->pod.m_evasionChance;
}

float Unit::GetCritResistance()
{
	return unitTemplateData->pod.m_critResistance;
}

std::string Unit::GetFBXName() const
{
	if (!skinnedMeshGameObject)
	{
		return std::string();
	}

	return skinnedMeshGameObject->getName();
}
Vector3d Unit::GetRandomPositionInsideCapsuleCollider()
{
	const auto& pod = GetUnitTemplateData().pod;
	auto unitPos = GetTransform()->GetWorldPosition();
	return unitPos + Vector3d{ math::Random::GetRandomFloat(pod.collisionSize * 0.5f), pod.collisionSize + math::Random::GetRandomFloat(0 , pod.collisionHeight * 0.7f), math::Random::GetRandomFloat(pod.collisionSize * 0.f) };
}
void Unit::EraseBuff(UnitBuffType buffType)
{
	if (buffs.find(buffType) != buffs.end())
		buffs.find(buffType)->second->OnEnd();
	buffs.erase(buffType);
}
void Unit::Damaged(std::weak_ptr<Unit> opponentUnit, float opponentDmg, DamageType damageType)
{
	const auto& gc = GlobalConstant::GetSingletonInstance().pod;
	switch (damageType)
	{
		case DamageType::Attack:
			// 치명타의 경우 치명타 배율은 이미 opponentDmg에 반영된 것으로 친다.
		case DamageType::AttackCrit:
			if (math::Random::ByRandomChance(GetEvasionChance()))
			{
				// 공격 빗나감
				opponentDmg = 0;
				ShowMissedUI();
				return;
			}

			if (damageType == DamageType::AttackCrit)
			{
				opponentDmg *= 1 + ((opponentUnit.lock()->GetCritMultiplier() - 1) * GetCritResistance());
			}
			opponentDmg *= 1 - GetArmor() * 0.01f;
			opponentUnit.lock()->onAttackHit(GetWeakPtr<Unit>());
			break;
		case DamageType::Skill:
			break;
		default:
			break;
	}

	if (opponentDmg > gc.dmgIndicatorMinDamage)
	{
		if (damageType == DamageType::AttackCrit)
		{
			ContentsCoroutine::Instance().StartCoroutine(DmgIndicatorCoroutine(opponentDmg, dmgCriticalUIID));
		}
		else
		{
			ContentsCoroutine::Instance().StartCoroutine(DmgIndicatorCoroutine(opponentDmg, dmgDefaultUIID));
		}
	}

	onDamagedFromUnit(opponentUnit);
	Damaged(opponentDmg);
}

void Unit::Damaged(float dmg)
{
	SetCurrentHp(currentHitPoint -= dmg);
	onDamaged();
}

void Unit::Heal(float healingPoint)
{
	// 최대 체력이면 x
	SetCurrentHp(currentHitPoint += healingPoint);
	if (currentHitPoint >= unitTemplateData->pod.max_Health)
		SetCurrentHp(unitTemplateData->pod.max_Health);

	coroutineHealEffect = StartCoroutine(HealEffectCoroutine());
	coroutineHealEffect.lock()->PushDestroyCallBack([this]()
		{
			FBXPool::Instance().Return(healVFX);
		});
}

yunutyEngine::coroutine::Coroutine Unit::HealEffectCoroutine()
{
	if (!healVFX.expired())
	{
		FBXPool::Instance().Return(healVFX);
	}
	co_await std::suspend_always{};

	healVFX = FBXPool::Instance().Borrow("VFX_Buff_Healing");
	auto vfxAnimator = healVFX.lock()->AcquireVFXAnimator();
	vfxAnimator.lock()->SetAutoActiveFalse();
	vfxAnimator.lock()->Init();
	vfxAnimator.lock()->Play();

	healVFX.lock()->GetTransform()->SetWorldPosition(Vector3d(GetTransform()->GetWorldPosition().x, 0, GetTransform()->GetWorldPosition().z));
	healVFX.lock()->GetTransform()->SetWorldRotation(GetTransform()->GetWorldRotation());
	co_await std::suspend_always{};

	coroutine::ForSeconds forseconds{ vfxAnimator.lock()->GetDuration() };

	while (forseconds.Tick())
	{
		co_await std::suspend_always{};
		healVFX.lock()->GetTransform()->SetWorldPosition(Vector3d(GetTransform()->GetWorldPosition().x, 0, GetTransform()->GetWorldPosition().z));
		healVFX.lock()->GetTransform()->SetWorldRotation(GetTransform()->GetWorldRotation());
	}

	co_return;
}

void Unit::SetCurrentHp(float p_newHp)
{
	currentHitPoint = std::fmax(0, p_newHp);
	if (p_newHp <= 0 && isAlive)
	{
		liveCountLeft--;
		SetIsAlive(false);
	}
	for (auto unitStatusUI : unitStatusUIs)
	{
		if (!unitStatusUI.expired())
		{
			if (isAlive && !unitStatusUI.lock()->GetUIEnabled() && currentHitPoint != unitTemplateData->pod.max_Health && (unitStatusUI.lock()->runtimeFlags & UnitStatusBarFlag::EnableAfterDamaged))
			{
				unitStatusUI.lock()->EnableElement();
			}
			unitStatusUI.lock()->GetLocalUIsByEnumID().at(UIEnumID::StatusBar_HP_Fill)->adjuster->SetTargetFloat(1 - currentHitPoint / unitTemplateData->pod.max_Health);
			if (unitStatusUI.lock()->GetLocalUIsByEnumID().contains(UIEnumID::StatusBar_HP_Number_Current))
			{
				unitStatusUI.lock()->GetLocalUIsByEnumID().at(UIEnumID::StatusBar_HP_Number_Current)->SetNumber(currentHitPoint);
				unitStatusUI.lock()->GetLocalUIsByEnumID().at(UIEnumID::StatusBar_HP_Number_Current)->SetNumber(unitTemplateData->pod.max_Health);
			}
		}
	}
	if (!unitStatusPortraitUI.expired())
	{
		unitStatusPortraitUI.lock()->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_HP_Fill)->adjuster->SetTargetFloat(currentHitPoint / unitTemplateData->pod.max_Health);
		unitStatusPortraitUI.lock()->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_PortraitBloodOverlay)->adjuster->SetTargetFloat(1 - currentHitPoint / unitTemplateData->pod.max_Health);
		unitStatusPortraitUI.lock()->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_HP_Number_Current)->SetNumber(currentHitPoint);
		unitStatusPortraitUI.lock()->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_HP_Number_Max)->SetNumber(unitTemplateData->pod.max_Health);
	}
	if (!unitStatusPortraitUI2.expired())
	{
		unitStatusPortraitUI2.lock()->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_HP_Fill)->adjuster->SetTargetFloat(currentHitPoint / unitTemplateData->pod.max_Health);
		unitStatusPortraitUI2.lock()->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_PortraitBloodOverlay)->adjuster->SetTargetFloat(1 - currentHitPoint / unitTemplateData->pod.max_Health);
		unitStatusPortraitUI2.lock()->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_HP_Number_Current)->SetNumber(currentHitPoint);
		unitStatusPortraitUI2.lock()->GetLocalUIsByEnumID().at(UIEnumID::CharInfo_HP_Number_Max)->SetNumber(unitTemplateData->pod.max_Health);
	}
	if (currentHitPoint <= unitTemplateData->pod.max_Health * 0.5f || !GetGameObject()->GetActive())
	{
		PlayerPortraitUIs::SetPortraitHurt((PlayerCharacterType::Enum)unitTemplateData->pod.playerUnitType.enumValue);
	}
	else
	{
		PlayerPortraitUIs::SetPortraitIdle((PlayerCharacterType::Enum)unitTemplateData->pod.playerUnitType.enumValue);
	}
}

UnitOrderType Unit::GetPendingOrderType() const
{
	return pendingOrderType;
}

float Unit::GetUnitCurrentHp() const
{
	return currentHitPoint;
}

float Unit::GetUnitMaxHp() const
{
	return unitTemplateData->pod.max_Health;
}

void Unit::ProgressInitialize()
{
	CurrentProgressSave();
}

void Unit::CurrentProgressSave()
{
	if (GetUnitTemplateData().pod.unitControllerType.enumValue == UnitControllerType::PLAYER)
	{
		capturedLastPosition = GetTransform()->GetWorldPosition();
		liveCountLeftCaptured = liveCountLeft;
	}
}

void Unit::Recovery()
{
	if (GetUnitTemplateData().pod.unitControllerType.enumValue == UnitControllerType::PLAYER)
	{
		SetCurrentHp(GetUnitTemplateData().pod.max_Health);
		SetIsAlive(true);
		Relocate(capturedLastPosition);
		liveCountLeft = liveCountLeftCaptured;
	}
	else if (GetUnitTemplateData().pod.unitControllerType.enumValue == UnitControllerType::HEART_QUEEN)
	{
		if (unitData == nullptr)
		{
			return;
		}

		Reset();		
		Relocate(Vector3d(unitData->pod.position));
		GetTransform()->SetWorldPosition(Vector3d(unitData->pod.position));
		SetRotation(Quaternion(unitData->pod.rotation.w, unitData->pod.rotation.x, unitData->pod.rotation.y, unitData->pod.rotation.z), 0);

		for (auto unitStatusUI : unitStatusUIs)
		{
			if (auto status = unitStatusUI.lock())
			{
				status->DisableElement();
			}
			if (unitStatusUI.lock()->runtimeFlags & UnitStatusBarFlag::ControlWithReallyDisabled)
			{
				unitStatusUI.lock()->reallyDisabled = true;
			}
		}
	}
	else
	{
		ReturnToPool();
		for (auto unitStatusUI : unitStatusUIs)
		{
			if (auto status = unitStatusUI.lock())
			{
				status->DisableElement();
			}
			if (unitStatusUI.lock()->runtimeFlags & UnitStatusBarFlag::ControlWithReallyDisabled)
			{
				unitStatusUI.lock()->reallyDisabled = true;
			}
		}
	}
}

void Unit::KnockBack(Vector3d targetPosition, float knockBackDuration)
{
	if (IsAlive())
	{
		DeleteCoroutine(coroutineKnockBack);
		coroutineKnockBack = StartCoroutine(KnockbackCoroutine(referenceParalysis.Acquire(), targetPosition, knockBackDuration));
	}
}

void Unit::KnockBackRelativeVector(Vector3d relativeVector, float knockBackDuration)
{
	if (IsAlive())
	{
		DeleteCoroutine(coroutineKnockBack);
		coroutineKnockBack = StartCoroutine(KnockbackCoroutine(referenceParalysis.Acquire(), relativeVector, knockBackDuration, true));
	}
}

void Unit::Paralyze(float paralyzeDuration)
{
	StartCoroutine(referenceParalysis.AcquireForSecondsCoroutine(paralyzeDuration));
	auto paralCoroutine = StartCoroutine(ParalyzeEffectCoroutine(paralyzeDuration));
	paralCoroutine.lock()->PushDestroyCallBack([this]()
		{
			FBXPool::Instance().Return(paralysisVFX);
		});
}

yunutyEngine::coroutine::Coroutine Unit::ParalyzeEffectCoroutine(float paralyzeDuration)
{
	paralysisVFX = FBXPool::Instance().Borrow("VFX_Monster_HitCC");
	co_await std::suspend_always{};

	auto paralysisEffectAnimator = paralysisVFX.lock()->AcquireVFXAnimator();
	paralysisEffectAnimator.lock()->SetAutoActiveFalse();
	paralysisEffectAnimator.lock()->Init();
	paralysisEffectAnimator.lock()->Play();
	paralysisEffectAnimator.lock()->SetLoop(true);

	float localScaleInfo = 1.0f;
	if (this->GetUnitTemplateData().pod.unitControllerType.enumValue == UnitControllerType::Enum::HEART_QUEEN)
	{
		localScaleInfo = 1.5f;
	}
	else if (this->GetUnitTemplateData().pod.unitControllerType.enumValue == UnitControllerType::Enum::MELEE_ELITE)
	{
		localScaleInfo = 1.5f;
	}
	else if (this->GetUnitTemplateData().pod.unitControllerType.enumValue == UnitControllerType::Enum::RANGED_ELITE)
	{
		localScaleInfo = 1.5f;
	}
	paralysisVFX.lock()->GetTransform()->SetWorldPosition(GetTransform()->GetWorldPosition());
	paralysisVFX.lock()->GetTransform()->SetWorldRotation(GetTransform()->GetWorldRotation());
	paralysisVFX.lock()->GetTransform()->SetWorldScale(GetTransform()->GetWorldScale() * localScaleInfo);

	coroutine::ForSeconds forSeconds = paralyzeDuration;

	while (forSeconds.Tick())
	{
		co_await std::suspend_always{};
		paralysisVFX.lock()->GetTransform()->SetWorldPosition(GetTransform()->GetWorldPosition());
		paralysisVFX.lock()->GetTransform()->SetWorldRotation(GetTransform()->GetWorldRotation());
	}

	co_return;
}

yunutyEngine::coroutine::Coroutine Unit::KnockbackCoroutine(std::shared_ptr<Reference::Guard> paralysisGuard, Vector3d targetPosition, float knockBackDuration, bool relative)
{
	coroutine::ForSeconds forSeconds{ knockBackDuration };
	const auto& constant = GlobalConstant::GetSingletonInstance().pod;
	constant.gravitySpeed;
	// y = vy0 * t - 0.5 * g * t^2
	// vy0 = (0 + 0.5 * g * t^2) / t
	float vy0 = 0.5 * constant.gravitySpeed * knockBackDuration;
	float y;
	Vector3d startPos = GetTransform()->GetWorldPosition();

	if (relative)
	{
		targetPosition = startPos + targetPosition;
	}

	targetPosition = SingleNavigationField::Instance().GetClosestPointOnField(targetPosition);

	navAgentComponent.lock()->Relocate(targetPosition);

	while (forSeconds.Tick())
	{
		bool isNavAgentActive = navAgentComponent.lock()->GetActive();
		navAgentComponent.lock()->MoveTo(targetPosition + Vector3d::one);
		y = vy0 * forSeconds.Elapsed() - 0.5 * constant.gravitySpeed * forSeconds.Elapsed() * forSeconds.Elapsed();
		Vector3d pos = Vector3d::Lerp(startPos, navAgentComponent.lock()->GetTransform()->GetWorldPosition(), forSeconds.ElapsedNormalized());
		pos.y = y;
		GetTransform()->SetWorldPosition(pos);
		co_await std::suspend_always{};
	}
	co_return;
}
void Unit::PlayAnimation(UnitAnimType animType, Animation::PlayFlag playFlag)
{
	if (playingBattleAnim)
	{
		switch (animType)
		{
			case UnitAnimType::Idle: animType = UnitAnimType::BattleIdle; break;
			case UnitAnimType::Move: animType = UnitAnimType::BattleMove; break;
		}
	}
	auto anim = wanderResources::GetAnimation(unitTemplateData->pod.skinnedFBXName, animType);

	if (playFlag & Animation::PlayFlag_::NonRedundant && animatorComponent.lock()->GetGI().GetCurrentAnimation() == anim)
	{
		return;
	}

	// 새로 애니메이션을 재생할지, 애니메이션 블렌딩을 시도할지
	if (animatorComponent.lock()->GetGI().GetCurrentAnimation() == nullptr
		|| animatorComponent.lock()->GetGI().GetCurrentAnimation() == anim
		|| !(playFlag & Animation::PlayFlag_::Blending))
	{
		animatorComponent.lock()->Play(anim);
	}
	else
	{
		auto prevType = wanderResources::GetAnimationType(unitTemplateData->pod.skinnedFBXName, animatorComponent.lock()->GetGI().GetCurrentAnimation());
		animatorComponent.lock()->ChangeAnimation(anim, unitTemplateData->pod.animationBlendMap.at(std::pair((int)prevType, (int)animType)), 1);
	}

	if (playFlag & Animation::PlayFlag_::Repeat)
	{
		blendWithDefaultAnimTrigger = true;
		defaultAnimationType = animType;
		//defaultAnimation = anim;
	}
}
void Unit::BlendWithDefaultAnimation()
{
	blendWithDefaultAnimTrigger = true;
}
void Unit::SetDefaultAnimation(UnitAnimType animType)
{
	defaultAnimationType = animType;
}
float normalizeAngle(float angle) {
	while (angle < 0) angle += 360;
	while (angle >= 360) angle -= 360;
	return angle;
}
void Unit::SetDesiredRotation(const Vector3d& facingDirection)
{
	static constexpr float epsilon = 1.0f;
	if (facingDirection.MagnitudeSqr() < epsilon * Time::GetDeltaTime() * localTimeScale * Time::GetDeltaTime() * localTimeScale)
		return;
	desiredRotation = std::atan2(facingDirection.z, facingDirection.x) * math::Rad2Deg;
}
std::weak_ptr<coroutine::Coroutine> Unit::SetRotation(const Quaternion& targetRotation, float rotatingTime)
{
	return SetRotation(targetRotation * -Vector3d::forward, rotatingTime);
}
std::weak_ptr<coroutine::Coroutine> Unit::SetRotation(const Vector3d& facingDirection, float rotatingTime)
{
	desiredRotation = std::atan2(facingDirection.z, facingDirection.x) * math::Rad2Deg;
	return SetRotation(desiredRotation, rotatingTime);
}
std::weak_ptr<coroutine::Coroutine> Unit::SetRotation(float facingAngle, float rotatingTime)
{
	return StartCoroutine(SettingRotation(facingAngle, rotatingTime));
}
coroutine::Coroutine Unit::SettingRotation(float facingAngle, float rotatingTime)
{
	auto pauseGuard = referencePause.Acquire();
	desiredRotation = facingAngle;
	if (rotatingTime != 0)
	{
		currentRotationSpeed = std::fabs(getDeltaAngle(desiredRotation - currentRotation)) / rotatingTime;
	}
	else
	{
		currentRotationSpeed = 1000000000;
	}

	wanderUtils::UnitCoroutine::ForSecondsFromUnit waitRotating{ GetWeakPtr<Unit>(), rotatingTime };

	while (waitRotating.Tick())
	{
		co_await std::suspend_always();
	}
	co_await std::suspend_always();

	currentRotationSpeed = unitTemplateData->pod.rotationSpeed;
}
void Unit::SetIsAlive(bool isAlive)
{
	this->isAlive = isAlive;
}
void Unit::UpdateRotation()
{
	currentRotation = normalizeAngle(currentRotation);
	desiredRotation = normalizeAngle(desiredRotation);
	float difference = desiredRotation - currentRotation;
	difference = getDeltaAngle(difference);

	if (std::fabs(difference) < currentRotationSpeed * Time::GetDeltaTime() * localTimeScale) {
		currentRotation = desiredRotation;
	}
	else if (difference > 0) {
		currentRotation = normalizeAngle(currentRotation + currentRotationSpeed * Time::GetDeltaTime() * localTimeScale);
	}
	else {
		currentRotation = normalizeAngle(currentRotation - currentRotationSpeed * Time::GetDeltaTime() * localTimeScale);
	}
	GetTransform()->SetWorldRotation(Vector3d(0, -currentRotation + 90, 0));
}
void Unit::OnContentsStop()
{
	ReturnToPool();
}
void Unit::OnEnable()
{
	navAgentComponent.lock()->GetGameObject()->SetSelfActive(true);
}
void Unit::OnDisable()
{
	navAgentComponent.lock()->GetGameObject()->SetSelfActive(false);
}
void Unit::Start()
{
}
void Unit::Relocate(const Vector3d& pos)
{
	navAgentComponent.lock()->Relocate(pos);
	//OrderHold();
}
void Unit::OrderMove(Vector3d position)
{
	static constexpr float epsilon = 0.1f;
	if (DistanceSquare(position) < epsilon)
	{
		return;
	}
	pendingOrderType = UnitOrderType::Move;
	moveDestination = position;
	//moveDestination = position;
}
void Unit::OrderAttackMove(Vector3d position)
{
	pendingOrderType = UnitOrderType::AttackMove;
	attackMoveDestination = position;
}
void Unit::OrderAttackMove()
{
	OrderAttackMove(GetTransform()->GetWorldPosition());
}
void Unit::OrderAttack(std::weak_ptr<Unit> opponent)
{
	pendingOrderType = UnitOrderType::AttackUnit;
	pendingTargetUnit = opponent;
	if (pendingTargetUnit.lock().get() == this)
	{
		int a = 1;
	}
}
void Unit::OrderHold()
{
	pendingOrderType = UnitOrderType::Hold;
}
Vector3d Unit::FromTo(std::weak_ptr<Unit> a, std::weak_ptr<Unit> b)
{
	return FromTo(a.lock().get(), b.lock().get());
}
Vector3d Unit::FromTo(Unit* a, Unit* b)
{
	return b->GetTransform()->GetWorldPosition() - a->GetTransform()->GetWorldPosition();
}
float Unit::Distance(std::weak_ptr<Unit> a, std::weak_ptr<Unit> b)
{
	return Distance(a.lock().get(), b.lock().get());
}
float Unit::DistanceSquare(std::weak_ptr<Unit> a, std::weak_ptr<Unit> b)
{
	return DistanceSquare(a.lock().get(), b.lock().get());
}
float Unit::Distance(Unit* a, Unit* b)
{
	return sqrtf(DistanceSquare(a, b));
}
float Unit::DistanceSquare(Unit* a, Unit* b)
{
	return (a->GetTransform()->GetWorldPosition() - b->GetTransform()->GetWorldPosition()).MagnitudeSqr();
}
float Unit::Distance(Unit* a, const Vector3d& worldPos)
{
	return sqrtf(DistanceSquare(a, worldPos));
}
float Unit::DistanceSquare(Unit* a, const Vector3d& worldPos)
{
	return (a->GetTransform()->GetWorldPosition() - worldPos).MagnitudeSqr();
}
float Unit::QuaternionToEastAngle(const Quaternion& rotation)
{
	auto&& forward = rotation.Forward();
	return 180 + std::atan2f(forward.z, forward.x) * math::Rad2Deg;
}
Quaternion Unit::EastAngleToQuaternion(const float rotation)
{
	return Quaternion();
}
bool Unit::GetPauseAll()
{
	return pauseAll;
}
void Unit::SetPauseAll(bool pause)
{
	pauseAll = pause;
}
float Unit::GetDesiredRotation()
{
	return desiredRotation;
}
float Unit::Distance(const Vector3d& worldPos)
{
	return Unit::Distance(this, worldPos);
}
float Unit::DistanceSquare(const Vector3d& worldPos)
{
	return Unit::DistanceSquare(this, worldPos);
}
// 유닛과 관련된 객체들을 모조리 생성
void Unit::Init(const application::editor::Unit_TemplateData* unitTemplateData)
{
	this->unitTemplateData = unitTemplateData;
	attackRange = GetGameObject()->AddGameObject()->AddComponentAsWeakPtr<UnitAcquisitionSphereCollider>();
	acquisitionRange = GetGameObject()->AddGameObject()->AddComponentAsWeakPtr<UnitAcquisitionSphereCollider>();
	attackRange.lock()->owner = GetWeakPtr<Unit>();
	acquisitionRange.lock()->owner = GetWeakPtr<Unit>();
	navAgentComponent = Scene::getCurrentScene()->AddGameObject()->AddComponentAsWeakPtr<NavigationAgent>();
	navObstacle = navAgentComponent.lock()->GetGameObject()->AddComponentAsWeakPtr<NavigationObstacle>();
	navObstacle.lock()->SetObstacleType(NavigationObstacle::ObstacleType::Cylinder);
	teamIndex = unitTemplateData->pod.playerUnitType.enumValue == PlayerCharacterType::None ? 2 : PlayerController::playerTeamIndex;
	skinnedMeshGameObject = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX(unitTemplateData->pod.skinnedFBXName);
	burnEffect = skinnedMeshGameObject->AddComponentAsWeakPtr<BurnEffect>();
	burnEffect.lock()->Init();
	animatorComponent = skinnedMeshGameObject->GetComponentWeakPtr<graphics::Animator>();
	SetSkinnedMeshRenderer(skinnedMeshGameObject);
	skinnedMeshGameObject->SetParent(GetGameObject());
	skinnedMeshGameObject->GetTransform()->SetLocalPosition(Vector3d::zero);
	skinnedMeshGameObject->GetTransform()->SetLocalRotation(Quaternion{ {0,180,0} });
	skinnedMeshGameObject->GetTransform()->SetLocalScale(Vector3d::one * unitTemplateData->pod.unit_scale);
	//wanderResources::PushAnimations(animatorComponent.lock().get(), unitTemplateData->pod.skinnedFBXName);
	unitCollider = GetGameObject()->AddGameObject()->AddComponentAsWeakPtr<UnitCapsuleCollider>();
	unitCollider.lock()->owner = GetWeakPtr<Unit>();
	unitCollider.lock()->GetTransform()->SetLocalRotation(Vector3d{ {0,0,90} });
	auto rigidBody = unitCollider.lock()->GetGameObject()->AddComponentAsWeakPtr<physics::RigidBody>();
	rigidBody.lock()->SetAsKinematic(true);
	InitBehaviorTree();
	// 애니메이션에 이벤트 삽입
	auto& ptm = particle::ParticleTool_Manager::GetSingletonInstance();
	const yunuGI::IResourceManager* resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
	std::wstring fbxNameW = yutility::GetWString(unitTemplateData->pod.skinnedFBXName);
	//fbxNameW.assign(pod.templateData->pod.skinnedFBXName.begin(), pod.templateData->pod.skinnedFBXName.end());
	//auto debugMesh = AttachDebugMesh(navAgentComponent.lock()->GetGameObject(), DebugMeshType::Capsule);
	//debugMesh->GetTransform()->SetLocalScale(Vector3d(unitTemplateData->pod.collisionSize * 2, unitTemplateData->pod.collisionHeight, unitTemplateData->pod.collisionSize * 2));
	//debugMesh->GetTransform()->SetLocalPosition(Vector3d::up * unitTemplateData->pod.collisionHeight * 0.5f);

	auto debugMesh = AttachDebugMesh(unitCollider.lock()->GetGameObject()->AddGameObject(), DebugMeshType::Capsule, yunuGI::Color::pink());
	debugMesh->GetTransform()->SetLocalScale(Vector3d(unitTemplateData->pod.collisionSize, unitTemplateData->pod.collisionHeight, unitTemplateData->pod.collisionSize));
	debugMesh->GetTransform()->SetLocalRotation(Vector3d{ {0,0,90} });

	/// Particle Setting
	for (auto& eachPI : ptm.GetChildrenParticleInstanceList(unitTemplateData->pod.skinnedFBXName))
	{
		auto pObj = GetGameObject()->AddGameObject();
		auto sptr = eachPI.lock();
		pObj->GetTransform()->SetLocalPosition(Vector3d(sptr->offsetPos.x, sptr->offsetPos.y, -sptr->offsetPos.z));
		pObj->GetTransform()->SetLocalRotation(Quaternion(Vector3d(0, 180, 0)) * sptr->rotation);
		pObj->GetTransform()->SetLocalScale(sptr->scale);
		pObj->setName(sptr->name);
		auto pr = pObj->AddComponent<graphics::ParticleRenderer>();
		pr->SetParticleShape((yunutyEngine::graphics::ParticleShape)sptr->particleData.shape);
		pr->SetParticleMode((yunutyEngine::graphics::ParticleMode)sptr->particleData.particleMode);
		pr->SetLoop(sptr->particleData.isLoop);
		pr->SetDuration(sptr->particleData.duration);
		pr->SetLifeTime(sptr->particleData.lifeTime);
		pr->SetSpeed(sptr->particleData.speed);
		pr->SetStartScale(sptr->particleData.startScale);
		pr->SetEndScale(sptr->particleData.endScale);
		pr->SetMaxParticle(sptr->particleData.maxParticle);
		pr->SetPlayAwake(sptr->particleData.playAwake);
		pr->SetIsApplyRoot(sptr->particleData.isApplyRoot);
		pr->SetRadius(sptr->particleData.radius);
		pr->SetAngle(sptr->particleData.angle);

		pr->SetRateOverTime(sptr->particleData.rateOverTime);

		pr->SetBurstsCount(sptr->particleData.burstsCount);
		pr->SetInterval(sptr->particleData.interval);

		pr->SetStartAngle(sptr->particleData.startAngle);
		pr->SetEndAngle(sptr->particleData.endAngle);
		pr->SetIsRandomScale(sptr->particleData.isRandomScale);
		pr->SetIsRandomAngle(sptr->particleData.isRandomAngle);
		pr->SetIsAlphaDiminish(sptr->particleData.isAlphaDiminish);

		std::wstring texturePath;
		texturePath.assign(sptr->particleData.texturePath.begin(), sptr->particleData.texturePath.end());
		auto texturePtr = resourceManager->GetTexture(texturePath);
		if (texturePtr)
		{
			pr->SetTexture(texturePtr);
		}

		pObj->SetSelfActive(false);
	}

	/// Animation Event Setting
	auto& list = resourceManager->GetFBXAnimationList(fbxNameW);
	for (auto& each : list)
	{
		std::string aniName;
		aniName.assign(each->GetName().begin(), each->GetName().end());

		for (auto& eventWeak : ptm.GetAnimationEventList(ptm.GetMatchingIAnimation(unitTemplateData->pod.skinnedFBXName, aniName)))
		{
			auto event = eventWeak.lock();
			auto type = event->GetType();
			switch (type)
			{
				case application::AnimationEventType::GameObject_ActivateEvent:
				{
					auto ptr = static_cast<GameObject_ActivateEvent*>(event.get());
					GameObject* particle = nullptr;
					for (auto& child : GetGameObject()->GetChildren())
					{
						if (child->getName() == ptr->objName)
						{
							particle = child;
							break;
						}
					}

					animatorComponent.lock()->PushAnimationWithFunc(each, event->frame, [=]()
						{
							particle->SetSelfActive(true);
							auto ptr = particle->GetComponent<graphics::ParticleRenderer>();
							ptr->Play();
						});

					break;
				}
				case application::AnimationEventType::GameObject_DisabledEvent:
				{
					auto ptr = static_cast<GameObject_DisabledEvent*>(event.get());
					GameObject* particle = nullptr;
					for (auto& child : GetGameObject()->GetChildren())
					{
						if (child->getName() == ptr->objName)
						{
							particle = child;
							break;
						}
					}

					animatorComponent.lock()->PushAnimationWithFunc(each, event->frame, [=]()
						{
							particle->SetSelfActive(false);
						});

					break;
				}
				case application::AnimationEventType::GameObject_TransformEditEvent:
				{
					auto ptr = static_cast<GameObject_TransformEditEvent*>(event.get());
					GameObject* particle = nullptr;
					for (auto& child : GetGameObject()->GetChildren())
					{
						if (child->getName() == ptr->objName)
						{
							particle = child;
							break;
						}
					}

					for (int i = 0; i < each->GetTotalFrame(); i++)
					{
						animatorComponent.lock()->PushAnimationWithFunc(each, i, [=]()
							{
								auto& aem = AnimationEventManager::GetSingletonInstance();
								auto target = aem.GetLerpPoint(ptr->editData, i);
								auto pos = target->GetLocalPosition();
								particle->GetTransform()->SetLocalPosition(Vector3d(pos.x, pos.y, -pos.z));
								particle->GetTransform()->SetLocalRotation(Quaternion(Vector3d(0, 180, 0)) * target->GetLocalRotation());
								particle->GetTransform()->SetLocalScale(target->GetLocalScale());
							});
					}

					break;
				}
				case application::AnimationEventType::Sound_PlayOnceEvent:
				{
					auto ptr = static_cast<Sound_PlayOnceEvent*>(event.get());
					animatorComponent.lock()->PushAnimationWithFunc(each, event->frame, [=]()
						{
							SFXManager::SingleInstance().PlaySoundfile3D(ptr->rscPath, animatorComponent.lock()->GetGameObject()->GetTransform()->GetWorldPosition());
						});
					break;
				}
				case application::AnimationEventType::GameObject_AwakeEvent:
				{
					auto ptr = static_cast<GameObject_AwakeEvent*>(event.get());
					GameObject* particle = nullptr;
					for (auto& child : GetGameObject()->GetChildren())
					{
						if (child->getName() == ptr->objName)
						{
							particle = child;
							break;
						}
					}

					animatorComponent.lock()->PushAnimationWithFunc(each, event->frame, [=]()
						{
							auto ptr = particle->GetComponent<graphics::ParticleRenderer>();
							ptr->Reset();
						});

					break;
				}
				default:
					break;
			}
		}
	}
}

void Unit::Summon(const application::editor::UnitData* unitData)
{
	this->unitData = unitData;
	unitData->inGameUnit = GetWeakPtr<Unit>();

	GetTransform()->SetWorldPosition(Vector3d{ unitData->pod.position });
	navAgentComponent.lock()->SetActive(true);
	navObstacle.lock()->SetActive(true);
	navAgentComponent.lock()->GetTransform()->SetWorldPosition(Vector3d{ unitData->pod.position });
	navAgentComponent.lock()->AssignToNavigationField(&SingleNavigationField::Instance());
	navObstacle.lock()->AssignToNavigationField(&SingleNavigationField::Instance());

	ResetCallbacks();
	Reset();

	onAttack = unitData->onAttack;
	onAttackHit = unitData->onAttackHit;
	onDamaged = unitData->onDamaged;
	onCreated = unitData->onCreated;
	onRotationFinish = unitData->onRotationFinish;
	onStateEngage = unitData->onStateEngage;
	onStateExit = unitData->onStateExit;

	Summon(unitData->GetUnitTemplateData());

	Quaternion quat{ unitData->pod.rotation.w,unitData->pod.rotation.x,unitData->pod.rotation.y ,unitData->pod.rotation.z };
	auto forward = quat.Forward();
	desiredRotation = currentRotation = QuaternionToEastAngle(quat);
	//desiredRotation = currentRotation = 180 + std::atan2f(forward.z, forward.x) * math::Rad2Deg;
	coroutineBirth = StartCoroutine(BirthCoroutine());
}

void Unit::Summon(application::editor::Unit_TemplateData* td, const Vector3d& position, float rotation, bool instant)
{
	this->unitData = nullptr;

	ResetCallbacks();
	Reset();
	Summon(td);

	GetTransform()->SetWorldPosition(Vector3d{ position });
	navAgentComponent.lock()->GetTransform()->SetWorldPosition(Vector3d{ position });
	navAgentComponent.lock()->AssignToNavigationField(&SingleNavigationField::Instance());
	navObstacle.lock()->AssignToNavigationField(&SingleNavigationField::Instance());

	GetTransform()->SetWorldRotation(Vector3d{ 0,90 - rotation,0 });
	desiredRotation = currentRotation = rotation;

	if (instant)
	{
		onCreated();
	}
	else
	{
		coroutineBirth = StartCoroutine(BirthCoroutine());
	}
}
void Unit::Summon(application::editor::Unit_TemplateData* td, const Vector3d& position, const Quaternion& rotation, bool instant)
{
	auto finalRot = QuaternionToEastAngle(rotation);
	Summon(td, position, finalRot, instant);
}
void Unit::AddPassiveSkill(std::shared_ptr<PassiveSkill> skill)
{
	passiveSkill = skill;
	passiveSkill->owner = GetWeakPtr<Unit>();
	passiveSkill->Init(GetWeakPtr<Unit>());
}
// 패시브 스킬을 추가하며 유닛의 여러 콜백 함수에 영향을 미친다.
void Unit::Summon(application::editor::Unit_TemplateData* templateData)
{
	skinnedMeshGameObject->GetTransform()->SetLocalScale(Vector3d::one * unitTemplateData->pod.unit_scale);
	switch (unitTemplateData->pod.playerUnitType.enumValue)
	{
		case PlayerCharacterType::Robin:
			unitStatusPortraitUI = UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Robin)->GetWeakPtr<UIElement>();
			unitStatusPortraitUI2 = UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Robin_Left)->GetWeakPtr<UIElement>();
			AddPassiveSkill(std::static_pointer_cast<PassiveSkill>(std::make_shared<PassiveRobinBleed>()));
			break;
		case PlayerCharacterType::Ursula:
			unitStatusPortraitUI = UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Ursula)->GetWeakPtr<UIElement>();
			unitStatusPortraitUI2 = UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Ursula_Left)->GetWeakPtr<UIElement>();
			AddPassiveSkill(std::static_pointer_cast<PassiveSkill>(std::make_shared<PassiveUrsula>()));
			break;
		case PlayerCharacterType::Hansel:
			unitStatusPortraitUI = UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Hansel)->GetWeakPtr<UIElement>();
			unitStatusPortraitUI2 = UIManager::Instance().GetUIElementByEnum(UIEnumID::CharInfo_Hansel_Left)->GetWeakPtr<UIElement>();
			AddPassiveSkill(std::static_pointer_cast<PassiveSkill>(std::make_shared<PassiveHanselHeal>()));
			break;
		default:
			break;
	}
	unitStatusUIs.clear();
	switch (unitTemplateData->pod.unitStatusBar.enumValue)
	{
		case UnitStatusBarType::PLAYER_ROBIN:
		{
			auto unitStatusUI = UIManager::Instance().DuplicateUIElement(UIManager::Instance().GetUIElementByEnum(UIEnumID::StatusBar_Hero_Robin));
			unitStatusUIs.push_back(unitStatusUI);
			unitStatusUI.lock()->runtimeFlags = UnitStatusBarFlag::DestroyOnDeath | UnitStatusBarFlag::FollowUnit;
			break;
		}
		break;
		case UnitStatusBarType::PLAYER_URSULA:
		{
			auto unitStatusUI = UIManager::Instance().DuplicateUIElement(UIManager::Instance().GetUIElementByEnum(UIEnumID::StatusBar_Hero_Ursula));
			unitStatusUIs.push_back(unitStatusUI);
			unitStatusUI.lock()->runtimeFlags = UnitStatusBarFlag::DestroyOnDeath | UnitStatusBarFlag::FollowUnit;
			break;
		}
		break;
		case UnitStatusBarType::PLAYER_HANSEL:
		{
			auto unitStatusUI = UIManager::Instance().DuplicateUIElement(UIManager::Instance().GetUIElementByEnum(UIEnumID::StatusBar_Hero_Hansel));
			unitStatusUIs.push_back(unitStatusUI);
			unitStatusUI.lock()->runtimeFlags = UnitStatusBarFlag::DestroyOnDeath | UnitStatusBarFlag::FollowUnit;
			break;
		}
		break;
		case UnitStatusBarType::ENEMY:
		{
			auto unitStatusUI = UIManager::Instance().DuplicateUIElement(UIManager::Instance().GetUIElementByEnum(UIEnumID::StatusBar_MeleeEnemy));
			unitStatusUIs.push_back(unitStatusUI);
			unitStatusUI.lock()->DisableElement();
			unitStatusUI.lock()->runtimeFlags = UnitStatusBarFlag::DestroyOnDeath | UnitStatusBarFlag::FollowUnit | UnitStatusBarFlag::EnableAfterDamaged;
			break;
		}
		case UnitStatusBarType::ELITE:
		{
			auto unitStatusUI = UIManager::Instance().DuplicateUIElement(UIManager::Instance().GetUIElementByEnum(UIEnumID::StatusBar_Elite));
			unitStatusUIs.push_back(unitStatusUI);
			unitStatusUI.lock()->DisableElement();
			unitStatusUI.lock()->runtimeFlags = UnitStatusBarFlag::DestroyOnDeath | UnitStatusBarFlag::FollowUnit | UnitStatusBarFlag::EnableAfterDamaged;
			break;
		}
		case UnitStatusBarType::BOSS:
		{
			auto unitStatusUI1 = UIManager::Instance().GetUIElementByEnum(UIEnumID::StatusBar_Boss);
			auto unitStatusUI2 = UIManager::Instance().GetUIElementByEnum(UIEnumID::StatusBar_Boss_Tactic);
			unitStatusUIs.push_back(unitStatusUI1->GetWeakPtr<UIElement>());
			unitStatusUIs.push_back(unitStatusUI2->GetWeakPtr<UIElement>());
			unitStatusUI1->runtimeFlags = UnitStatusBarFlag::ControlWithReallyDisabled;
			unitStatusUI2->runtimeFlags = UnitStatusBarFlag::ControlWithReallyDisabled;
			break;
		}
		case UnitStatusBarType::BOSS_FRAME_LEFT:
		{
			auto unitStatusUI1 = UIManager::Instance().GetUIElementByEnum(UIEnumID::StatusBar_LeftDoor);
			auto unitStatusUI2 = UIManager::Instance().GetUIElementByEnum(UIEnumID::StatusBar_LeftDoor_Tactic);
			unitStatusUIs.push_back(unitStatusUI1->GetWeakPtr<UIElement>());
			unitStatusUIs.push_back(unitStatusUI2->GetWeakPtr<UIElement>());
			unitStatusUI1->reallyDisabled = false;
			unitStatusUI2->reallyDisabled = false;
			unitStatusUI1->runtimeFlags = UnitStatusBarFlag::ControlWithReallyDisabled;
			unitStatusUI2->runtimeFlags = UnitStatusBarFlag::ControlWithReallyDisabled;
			unitStatusUI1->EnableElement();
			unitStatusUI2->EnableElement();
			break;
		}
		case UnitStatusBarType::BOSS_FRAME_RIGHT:
		{
			auto unitStatusUI1 = UIManager::Instance().GetUIElementByEnum(UIEnumID::StatusBar_RightDoor);
			auto unitStatusUI2 = UIManager::Instance().GetUIElementByEnum(UIEnumID::StatusBar_RightDoor_Tactic);
			unitStatusUIs.push_back(unitStatusUI1->GetWeakPtr<UIElement>());
			unitStatusUIs.push_back(unitStatusUI2->GetWeakPtr<UIElement>());
			unitStatusUI1->reallyDisabled = false;
			unitStatusUI2->reallyDisabled = false;
			unitStatusUI1->runtimeFlags = UnitStatusBarFlag::ControlWithReallyDisabled;
			unitStatusUI2->runtimeFlags = UnitStatusBarFlag::ControlWithReallyDisabled;
			unitStatusUI1->EnableElement();
			unitStatusUI2->EnableElement();
			break;
		}
	}

	unitCollider.lock()->SetRadius(unitTemplateData->pod.collisionSize);
	unitCollider.lock()->SetHalfHeight(unitTemplateData->pod.collisionHeight * 0.5f);
	unitCollider.lock()->GetTransform()->SetLocalPosition(Vector3d::up * unitTemplateData->pod.collisionHeight * 0.5f * 1.5f);

	attackRange.lock()->SetRadius(unitTemplateData->pod.m_atkRadius);
	attackRange.lock()->SetColor(yunuGI::Color::red());
	acquisitionRange.lock()->SetRadius(unitTemplateData->pod.m_idRadius);
	acquisitionRange.lock()->SetColor(yunuGI::Color::gray());
	// 공격 범위
	//AttachDebugMesh(attackRange.lock()->GetGameObject()->AddGameObject(), DebugMeshType::Sphere)->GetTransform()->SetLocalScale(Vector3d{ 1,0.25,1 } *2 * unitTemplateData->pod.m_atkRadius);
	// 인식 범위
	//AttachDebugMesh(acquisitionRange.lock()->GetGameObject()->AddGameObject(), DebugMeshType::Sphere)->GetTransform()->SetLocalScale(Vector3d{ 1,0.25,1 } *2 * unitTemplateData->pod.m_idRadius);
	// 혹여나 플레이어 캐릭터라면 플레이어로 등록, 플레이어가 아니면 그냥 무시된다.
	switch (templateData->pod.unitControllerType.enumValue)
	{
		case UnitControllerType::PLAYER:
		{
			PlayerController::Instance().RegisterUnit(GetWeakPtr<Unit>());
			controllers.push_back(&PlayerController::Instance());
			break;
		}
		case UnitControllerType::ANGRY_MOB:
		{
			EnemyAggroController::Instance().RegisterUnit(GetWeakPtr<Unit>());
			controllers.push_back(&EnemyAggroController::Instance());
			break;
		}
		case UnitControllerType::HOLDER:
		{
			HolderController::Instance().RegisterUnit(GetWeakPtr<Unit>());
			controllers.push_back(&HolderController::Instance());
			break;
		}
		case UnitControllerType::MELEE_ELITE:
		{
			EnemyAggroController::Instance().RegisterUnit(GetWeakPtr<Unit>());
			controllers.push_back(&EnemyAggroController::Instance());
			MeleeEliteController::Instance().RegisterUnit(GetWeakPtr<Unit>());
			controllers.push_back(&MeleeEliteController::Instance());
			break;
		}
		case UnitControllerType::RANGED_ELITE:
		{
			EnemyAggroController::Instance().RegisterUnit(GetWeakPtr<Unit>());
			controllers.push_back(&EnemyAggroController::Instance());
			RangedEliteController::Instance().RegisterUnit(GetWeakPtr<Unit>());
			controllers.push_back(&RangedEliteController::Instance());
			break;
		}
		case UnitControllerType::RANGED_KITING:
		{
			EnemyAggroController::Instance().RegisterUnit(GetWeakPtr<Unit>());
			controllers.push_back(&EnemyAggroController::Instance());
			RangedKitingController::Instance().RegisterUnit(GetWeakPtr<Unit>());
			controllers.push_back(&RangedKitingController::Instance());
			break;
		}
		case UnitControllerType::RANGED_APPROACHING:
		{
			EnemyAggroController::Instance().RegisterUnit(GetWeakPtr<Unit>());
			controllers.push_back(&EnemyAggroController::Instance());
			RangedApproachingController::Instance().RegisterUnit(GetWeakPtr<Unit>());
			controllers.push_back(&RangedApproachingController::Instance());
			break;
		}
		case UnitControllerType::HEART_QUEEN:
		{
			BossController::Instance().RegisterUnit(GetWeakPtr<Unit>());
			controllers.push_back(&BossController::Instance());
			break;
		}
	}
	// 피해량 수치 폰트 설정
	switch (templateData->pod.dmgIndicatorFont.enumValue)
	{
		case UnitDamageFontType::Red:
			dmgDefaultUIID = UIEnumID::DamageIndicator_Default_RedFont;
			dmgCriticalUIID = UIEnumID::DamageIndicator_Critical_RedFont;
			break;
		case UnitDamageFontType::Blue:
			dmgDefaultUIID = UIEnumID::DamageIndicator_Default_BlueFont;
			dmgCriticalUIID = UIEnumID::DamageIndicator_Critical_BlueFont;
			break;
		case UnitDamageFontType::BlackAndWhite:
			dmgDefaultUIID = UIEnumID::DamageIndicator_Default_BlackWhiteFont;
			dmgCriticalUIID = UIEnumID::DamageIndicator_Critical_BlackWhiteFont;
			break;

	}
	currentRotationSpeed = unitTemplateData->pod.rotationSpeed;
	navAgentComponent.lock()->SetRadius(unitTemplateData->pod.collisionSize);
	navAgentComponent.lock()->SetSpeed(unitTemplateData->pod.m_unitSpeed);
	navObstacle.lock()->SetRadiusAndHeight(unitTemplateData->pod.collisionSize, 100);
	SetCurrentHp(unitTemplateData->pod.max_Health);
	if (GetTeamIndex() != PlayerController::playerTeamIndex)
	{
		onStateEngage.at(UnitBehaviourTree::Death).AddCallback([this]()
			{
				PlayerController::Instance().AddCombo();
				PlayerController::Instance().SetMana(PlayerController::Instance().GetMana() + unitTemplateData->pod.deathManaReward);
			});
	}
}
void Unit::Reset()
{
	SetIsAlive(true);
	SetCurrentHp(unitTemplateData->pod.max_Health);
	DeleteCoroutine(coroutineKnockBack);
	DeleteCoroutine(coroutineAttack);
	if (!coroutineSkill.expired())
	{
		if (onGoingSkill)
		{
			onGoingSkill->OnInterruption();
		}
		DeleteCoroutine(coroutineSkill);
		onGoingSkill.reset();
	}
	DeleteCoroutine(coroutineRevival);
	DeleteCoroutine(coroutineBirth);
	DeleteCoroutine(coroutineDeath);
	ClearCoroutines();
	passiveSkill.reset();
	pauseRequested = false;
	unpauseRequested = false;
	buffs.clear();
	liveCountLeft = unitTemplateData->pod.liveCount;
	currentTargetUnit.reset();
	if (!burnEffect.expired())
	{
		burnEffect.lock()->Reset();
	}
	currentOrderType = UnitOrderType::AttackMove;
	pendingOrderType = UnitOrderType::AttackMove;
	attackMoveDestination = moveDestination = GetGameObject()->GetTransform()->GetWorldPosition();
	//pendingAttackMoveDestination = attackMoveDestination;
	//pendingMoveDestination = moveDestination;

	isPaused = false;
	localTimeScale = 1.0f;

	ResetSharedRef();
}
void Unit::InitBehaviorTree()
{
	// 이 행동 트리에 대한 설계문서는 Document/프로그래밍 폴더 내부의 파일 "InWanderLand Behaviour tree.drawio"입니다.
	unitBehaviourTree[UnitBehaviourTree::Death].enteringCondtion = [this]()
		{
			return !referenceBlockDeath.BeingReferenced() && !isAlive && liveCountLeft < 0;
		};
	unitBehaviourTree[UnitBehaviourTree::Death].onEnter = [this]()
		{
			OnStateEngage<UnitBehaviourTree::Death>();
		};
	unitBehaviourTree[UnitBehaviourTree::Death].onExit = [this]()
		{
			OnStateExit<UnitBehaviourTree::Death>();
		};
	unitBehaviourTree[UnitBehaviourTree::Paralysis].enteringCondtion = [this]()
		{
			return referenceParalysis.BeingReferenced();
		};
	unitBehaviourTree[UnitBehaviourTree::Paralysis].onEnter = [this]()
		{
			OnStateEngage<UnitBehaviourTree::Paralysis>();
		};
	unitBehaviourTree[UnitBehaviourTree::Paralysis].onExit = [this]()
		{
			OnStateExit<UnitBehaviourTree::Paralysis>();
		};
	unitBehaviourTree[UnitBehaviourTree::Paralysis][UnitBehaviourTree::Knockback].enteringCondtion = [this]()
		{
			return !coroutineKnockBack.expired();
		};
	unitBehaviourTree[UnitBehaviourTree::Paralysis][UnitBehaviourTree::Knockback].onEnter = [this]()
		{
			OnStateEngage<UnitBehaviourTree::Knockback>();
		};
	unitBehaviourTree[UnitBehaviourTree::Paralysis][UnitBehaviourTree::Knockback].onExit = [this]()
		{
			OnStateExit<UnitBehaviourTree::Knockback>();
		};
	unitBehaviourTree[UnitBehaviourTree::Paralysis][UnitBehaviourTree::Knockback].onUpdate = [this]()
		{
			OnStateUpdate<UnitBehaviourTree::Knockback>();
		};
	unitBehaviourTree[UnitBehaviourTree::Paralysis][UnitBehaviourTree::Stun].enteringCondtion = [this]()
		{
			return true;
		};
	unitBehaviourTree[UnitBehaviourTree::Paralysis][UnitBehaviourTree::Stun].onEnter = [this]()
		{
			OnStateEngage<UnitBehaviourTree::Stun>();
		};
	unitBehaviourTree[UnitBehaviourTree::Paralysis][UnitBehaviourTree::Stun].onExit = [this]()
		{
			OnStateExit<UnitBehaviourTree::Stun>();
		};
	unitBehaviourTree[UnitBehaviourTree::Pause].enteringCondtion = [this]()
		{
			return referencePause.BeingReferenced() || (pauseAll && !unpauseRequested) || pauseRequested;
		};
	unitBehaviourTree[UnitBehaviourTree::Pause].onEnter = [this]()
		{
			OnStateEngage<UnitBehaviourTree::Pause>();
		};
	unitBehaviourTree[UnitBehaviourTree::Pause].onExit = [this]()
		{
			OnStateExit<UnitBehaviourTree::Pause>();
		};
	unitBehaviourTree[UnitBehaviourTree::Reviving].enteringCondtion = [this]()
		{
			return !isAlive;
		};
	unitBehaviourTree[UnitBehaviourTree::Reviving].onEnter = [this]()
		{
			OnStateEngage<UnitBehaviourTree::Reviving>();
		};
	unitBehaviourTree[UnitBehaviourTree::Reviving].onExit = [this]()
		{
			OnStateExit<UnitBehaviourTree::Reviving>();
		};
	unitBehaviourTree[UnitBehaviourTree::Skill].enteringCondtion = [this]()
		{
			return !coroutineSkill.expired() || (CanProcessOrder<UnitOrderType::Skill>() && pendingSkill.get());
		};
	unitBehaviourTree[UnitBehaviourTree::Skill].onEnter = [this]()
		{
			currentOrderType = pendingOrderType;
			pendingOrderType = UnitOrderType::None;
			OnStateEngage<UnitBehaviourTree::Skill>();
		};
	unitBehaviourTree[UnitBehaviourTree::Skill].onExit = [this]()
		{
			OnStateExit<UnitBehaviourTree::Skill>();
		};
	unitBehaviourTree[UnitBehaviourTree::Skill][UnitBehaviourTree::SkillOnGoing].enteringCondtion = [this]()
		{
			return !coroutineSkill.expired();
		};
	unitBehaviourTree[UnitBehaviourTree::Skill][UnitBehaviourTree::SkillOnGoing].onEnter = [this]()
		{
			OnStateEngage<UnitBehaviourTree::SkillOnGoing>();
		};
	unitBehaviourTree[UnitBehaviourTree::Skill][UnitBehaviourTree::SkillOnGoing].onExit = [this]()
		{
			OnStateExit<UnitBehaviourTree::SkillOnGoing>();
		};
	unitBehaviourTree[UnitBehaviourTree::Skill][UnitBehaviourTree::SkillCasting].enteringCondtion = [this]()
		{
			return DistanceTo(pendingSkill.get()->targetPos) < pendingSkill.get()->GetCastRange();
		};
	unitBehaviourTree[UnitBehaviourTree::Skill][UnitBehaviourTree::SkillCasting].onEnter = [this]()
		{
			OnStateEngage<UnitBehaviourTree::SkillCasting>();
		};
	unitBehaviourTree[UnitBehaviourTree::Skill][UnitBehaviourTree::SkillCasting].onExit = [this]()
		{
			OnStateExit<UnitBehaviourTree::SkillCasting>();
		};
	unitBehaviourTree[UnitBehaviourTree::Skill][UnitBehaviourTree::Move].enteringCondtion = [this]()
		{
			return true;
		};
	unitBehaviourTree[UnitBehaviourTree::Skill][UnitBehaviourTree::Move].onEnter = [this]()
		{
			moveDestination = pendingSkill.get()->targetPos;
			//attackMoveDestination = moveDestination;
			OnStateEngage<UnitBehaviourTree::Move>();
		};
	unitBehaviourTree[UnitBehaviourTree::Skill][UnitBehaviourTree::Move].onUpdate = [this]()
		{
			OnStateUpdate<UnitBehaviourTree::Move>();
		};
	unitBehaviourTree[UnitBehaviourTree::Skill][UnitBehaviourTree::Move].onExit = [this]()
		{
			OnStateExit<UnitBehaviourTree::Move>();
		};
	unitBehaviourTree[UnitBehaviourTree::Tactic].enteringCondtion = [this]()
		{
			return referenceTactic.BeingReferenced();
		};
	unitBehaviourTree[UnitBehaviourTree::Tactic].onEnter = [this]()
		{
			OnStateEngage<UnitBehaviourTree::Tactic>();
		};
	unitBehaviourTree[UnitBehaviourTree::Tactic].onExit = [this]()
		{
			OnStateExit<UnitBehaviourTree::Tactic>();
		};
	unitBehaviourTree[UnitBehaviourTree::Chasing].enteringCondtion = [this]()
		{
			return ((!pendingTargetUnit.expired() && pendingTargetUnit.lock()->IsAlive()) ||
				(!currentTargetUnit.expired() && currentTargetUnit.lock()->IsAlive())) &&
				CanProcessOrder<UnitOrderType::AttackUnit>();
		};
	unitBehaviourTree[UnitBehaviourTree::Chasing].onEnter = [this]()
		{
			currentOrderType = pendingOrderType;
			pendingOrderType = UnitOrderType::None;
			OnStateEngage<UnitBehaviourTree::Chasing>();
		};
	unitBehaviourTree[UnitBehaviourTree::Chasing].onExit = [this]()
		{
			OnStateExit<UnitBehaviourTree::Chasing>();
		};
	unitBehaviourTree[UnitBehaviourTree::Chasing].onUpdate = [this]()
		{
			OnStateUpdate<UnitBehaviourTree::Chasing>();
		};
	unitBehaviourTree[UnitBehaviourTree::Chasing][UnitBehaviourTree::Attack].enteringCondtion = [this]()
		{
			return attackRange.lock()->GetEnemies().contains(currentTargetUnit.lock().get()) || !coroutineAttack.expired();
		};
	unitBehaviourTree[UnitBehaviourTree::Chasing][UnitBehaviourTree::Attack].onEnter = [this]()
		{
			OnStateEngage<UnitBehaviourTree::Attack>();
		};
	unitBehaviourTree[UnitBehaviourTree::Chasing][UnitBehaviourTree::Attack].onExit = [this]()
		{
			OnStateExit<UnitBehaviourTree::Attack>();
		};
	unitBehaviourTree[UnitBehaviourTree::Chasing][UnitBehaviourTree::Attack].onUpdate = [this]()
		{
			OnStateUpdate<UnitBehaviourTree::Attack>();
		};
	unitBehaviourTree[UnitBehaviourTree::Chasing][UnitBehaviourTree::Move].enteringCondtion = [this]()
		{
			return true;
		};
	unitBehaviourTree[UnitBehaviourTree::Chasing][UnitBehaviourTree::Move].onEnter = [this]()
		{
			OnStateEngage<UnitBehaviourTree::Move>();
		};
	unitBehaviourTree[UnitBehaviourTree::Chasing][UnitBehaviourTree::Move].onExit = [this]()
		{
			OnStateExit<UnitBehaviourTree::Move>();
		};
	unitBehaviourTree[UnitBehaviourTree::Chasing][UnitBehaviourTree::Move].onUpdate = [this]()
		{
			moveDestination = GetAttackPosition(GetAttackTarget());
			OnStateUpdate<UnitBehaviourTree::Move>();
		};
	unitBehaviourTree[UnitBehaviourTree::Hold].enteringCondtion = [this]()
		{
			return CanProcessOrder<UnitOrderType::Hold>();
		};
	unitBehaviourTree[UnitBehaviourTree::Hold].onEnter = [this]()
		{
			currentOrderType = pendingOrderType;
			pendingOrderType = UnitOrderType::None;
			OnStateEngage<UnitBehaviourTree::Hold>();
		};
	unitBehaviourTree[UnitBehaviourTree::Hold].onExit = [this]()
		{
			OnStateExit<UnitBehaviourTree::Hold>();
		};
	unitBehaviourTree[UnitBehaviourTree::Hold][UnitBehaviourTree::Attack].enteringCondtion = [this]()
		{
			return !attackRange.lock()->GetEnemies().empty() || !coroutineAttack.expired();
		};
	unitBehaviourTree[UnitBehaviourTree::Hold][UnitBehaviourTree::Attack].onEnter = [this]()
		{
			OnStateEngage<UnitBehaviourTree::Attack>();
		};
	unitBehaviourTree[UnitBehaviourTree::Hold][UnitBehaviourTree::Attack].onExit = [this]()
		{
			OnStateExit<UnitBehaviourTree::Attack>();
		};
	unitBehaviourTree[UnitBehaviourTree::Hold][UnitBehaviourTree::Attack].onUpdate = [this]()
		{
			UpdateAttackTargetWithinRange();
			OnStateUpdate<UnitBehaviourTree::Attack>();
		};
	unitBehaviourTree[UnitBehaviourTree::Hold][UnitBehaviourTree::Stop].enteringCondtion = [this]()
		{
			return true;
		};
	unitBehaviourTree[UnitBehaviourTree::Hold][UnitBehaviourTree::Stop].onEnter = [this]()
		{
			OnStateEngage<UnitBehaviourTree::Stop>();
		};
	unitBehaviourTree[UnitBehaviourTree::Hold][UnitBehaviourTree::Stop].onExit = [this]()
		{
			OnStateExit<UnitBehaviourTree::Stop>();
		};
	unitBehaviourTree[UnitBehaviourTree::Move].enteringCondtion = [this]()
		{
			return CanProcessOrder<UnitOrderType::Move>();
		};
	unitBehaviourTree[UnitBehaviourTree::Move].onEnter = [this]()
		{
			currentOrderType = pendingOrderType;
			pendingOrderType = UnitOrderType::None;
			OnStateEngage<UnitBehaviourTree::Move>();
		};
	unitBehaviourTree[UnitBehaviourTree::Move].onExit = [this]()
		{
			OnStateExit<UnitBehaviourTree::Move>();
			if (pendingOrderType == UnitOrderType::None)
			{
				OrderHold();
				unitBehaviourTree.reAssessFlag = true;
			}
		};
	unitBehaviourTree[UnitBehaviourTree::Move].onUpdate = [this]()
		{
			OnStateUpdate<UnitBehaviourTree::Move>();
		};

	unitBehaviourTree[UnitBehaviourTree::AttackMove].enteringCondtion = [this]()
		{
			//return CanProceedOrder<UnitOrderType::AttackMove>();
			return true;
		};
	unitBehaviourTree[UnitBehaviourTree::AttackMove].onEnter = [this]()
		{
			//currentOrderType = pendingOrderType;
			currentOrderType = UnitOrderType::AttackMove;
			OnStateEngage<UnitBehaviourTree::AttackMove>();
		};
	unitBehaviourTree[UnitBehaviourTree::AttackMove].onExit = [this]()
		{
			OnStateExit<UnitBehaviourTree::AttackMove>();
		};
	unitBehaviourTree[UnitBehaviourTree::AttackMove][UnitBehaviourTree::Attack].enteringCondtion = [this]()
		{
			return !attackRange.lock()->GetEnemies().empty() || !coroutineAttack.expired();
		};
	unitBehaviourTree[UnitBehaviourTree::AttackMove][UnitBehaviourTree::Attack].onEnter = [this]()
		{
			OnStateEngage<UnitBehaviourTree::Attack>();
		};
	unitBehaviourTree[UnitBehaviourTree::AttackMove][UnitBehaviourTree::Attack].onExit = [this]()
		{
			OnStateExit<UnitBehaviourTree::Attack>();
		};
	unitBehaviourTree[UnitBehaviourTree::AttackMove][UnitBehaviourTree::Attack].onUpdate = [this]()
		{
			UpdateAttackTargetWithinRange();
			OnStateUpdate<UnitBehaviourTree::Attack>();
		};
	unitBehaviourTree[UnitBehaviourTree::AttackMove][UnitBehaviourTree::Move].enteringCondtion = [this]()
		{
			constexpr float epsilon = 0.1f;
			auto distance = (attackMoveDestination - GetTransform()->GetWorldPosition()).MagnitudeSqr();
			if (!acquisitionRange.lock()->GetEnemies().empty() || distance > epsilon)
			{
				int a = 3;
			}
			return !acquisitionRange.lock()->GetEnemies().empty() || distance > epsilon;
		};
	unitBehaviourTree[UnitBehaviourTree::AttackMove][UnitBehaviourTree::Move].onEnter = [this]()
		{
			OnStateEngage<UnitBehaviourTree::Move>();
		};
	unitBehaviourTree[UnitBehaviourTree::AttackMove][UnitBehaviourTree::Move].onExit = [this]()
		{
			OnStateExit<UnitBehaviourTree::Move>();
		};
	unitBehaviourTree[UnitBehaviourTree::AttackMove][UnitBehaviourTree::Move].onUpdate = [this]()
		{
			currentTargetUnit = GetClosestEnemy();
			moveDestination = currentTargetUnit.expired() ? attackMoveDestination : GetAttackPosition(currentTargetUnit);
			OnStateUpdate<UnitBehaviourTree::Move>();
		};
	unitBehaviourTree[UnitBehaviourTree::AttackMove][UnitBehaviourTree::Stop].enteringCondtion = [this]()
		{
			return true;
		};
	unitBehaviourTree[UnitBehaviourTree::AttackMove][UnitBehaviourTree::Stop].onEnter = [this]()
		{
			OnStateEngage<UnitBehaviourTree::Stop>();
		};
	unitBehaviourTree[UnitBehaviourTree::AttackMove][UnitBehaviourTree::Stop].onExit = [this]()
		{
			OnStateExit<UnitBehaviourTree::Stop>();
		};
}
Vector3d Unit::GetAttackPosition(std::weak_ptr<Unit> opponent)
{
	auto delta = opponent.lock()->GetTransform()->GetWorldPosition() - GetTransform()->GetWorldPosition();
	return opponent.lock()->GetTransform()->GetWorldPosition() - opponent.lock()->unitTemplateData->pod.collisionSize * delta.Normalized();
}
yunutyEngine::coroutine::Coroutine Unit::RevivalCoroutine(float revivalDelay)
{
	float birthAnimDuration = wanderResources::GetAnimation(unitTemplateData->pod.skinnedFBXName, UnitAnimType::Birth)->GetDuration();
	PlayAnimation(UnitAnimType::Death);
	SetDefaultAnimation(UnitAnimType::None);
	co_yield coroutine::WaitForSeconds(revivalDelay - birthAnimDuration);
	PlayAnimation(UnitAnimType::Birth);
	co_yield coroutine::WaitForSeconds(birthAnimDuration);
	SetCurrentHp(unitTemplateData->pod.max_Health);
	SetIsAlive(true);
	ApplyBuff(UnitBuffInvulenerability{ unitTemplateData->pod.revivalInvulnerableDuration });
	for (auto unitStatusUI : unitStatusUIs)
	{
		if (auto status = unitStatusUI.lock())
		{
			status->EnableElement();
		}
		if (unitStatusUI.lock()->runtimeFlags & UnitStatusBarFlag::ControlWithReallyDisabled)
		{
			unitStatusUI.lock()->reallyDisabled = false;
		}
	}
	co_return;
}
yunutyEngine::coroutine::Coroutine Unit::BirthCoroutine()
{
	if (unitTemplateData->pod.birthTime <= 0)
	{
		onCreated();
		co_return;
	}

	while (!IsPlayerUnit() && isPaused)
	{
		co_await std::suspend_always();
	}

	co_await std::suspend_always();

	const auto& gc = GlobalConstant::GetSingletonInstance().pod;
	auto pauseGuard = referencePause.Acquire();
	auto invulnerableGuard = referenceInvulnerable.Acquire();
	float animSpeed = wanderResources::GetAnimation(unitTemplateData->pod.skinnedFBXName, UnitAnimType::Birth)->GetDuration() / unitTemplateData->pod.birthTime;
	animatorComponent.lock()->GetGI().SetPlaySpeed(animSpeed);
	PlayAnimation(UnitAnimType::Birth, Animation::PlayFlag_::None);
	burnEffect.lock()->SetDuration(unitTemplateData->pod.birthTime);
	burnEffect.lock()->Appear();

	wanderUtils::UnitCoroutine::ForSecondsFromUnit forSeconds{ GetWeakPtr<Unit>(), unitTemplateData->pod.birthTime };

	while (forSeconds.Tick())
	{
		co_await std::suspend_always();
	}

	animatorComponent.lock()->GetGI().SetPlaySpeed(1);
	onCreated();
	co_return;
}
yunutyEngine::coroutine::Coroutine Unit::DeathCoroutine()
{
	if (unitTemplateData->pod.deathBurnTime <= 0)
	{
		ReturnToPool();
		co_return;
	}

	co_await std::suspend_always();

	auto pauseGuard = referencePause.Acquire();
	//float animSpeed = wanderResources::GetAnimation(unitTemplateData->pod.skinnedFBXName, UnitAnimType::Death)->GetDuration() / unitTemplateData->pod.deathBurnTime;
	//animatorComponent.lock()->GetGI().SetPlaySpeed(animSpeed);
	PlayAnimation(UnitAnimType::Death);
	co_yield coroutine::WaitForSeconds(unitTemplateData->pod.deathBurnOffset);
	if (!GetUnitTemplateData().pod.lingeringCorpse)
	{
		burnEffect.lock()->SetDuration(unitTemplateData->pod.deathBurnTime);
		burnEffect.lock()->Disappear();
	}
	for (auto unitStatusUI : unitStatusUIs)
	{
		if (auto status = unitStatusUI.lock())
		{
			status->DisableElement();
		}
		if (unitStatusUI.lock()->runtimeFlags & UnitStatusBarFlag::ControlWithReallyDisabled)
		{
			unitStatusUI.lock()->reallyDisabled = true;
		}
	}
	co_yield coroutine::WaitForSeconds(unitTemplateData->pod.deathBurnTime);
	//animatorComponent.lock()->GetGI().SetPlaySpeed(1);
	if (!GetUnitTemplateData().pod.lingeringCorpse)
		ReturnToPool();
	co_return;
}

yunutyEngine::coroutine::Coroutine Unit::AttackCoroutine(std::weak_ptr<Unit> opponent)
{
	//if (unitTemplateData->pod.skinnedFBXName == "SKM_HeartQueen")
	//{
	//	defaultAnimationType = UnitAnimType::None;
	//}

	//auto blockAttack = referenceBlockAttack.Acquire();
	defaultAnimationType = UnitAnimType::Idle;
	// 공격 애니메이션이 자연스럽게 맞물리기까지 필요한 최소시간
	float animMinimumTime = unitTemplateData->pod.m_attackPreDelay + unitTemplateData->pod.m_attackPostDelay;
	// 현재 공격주기
	float finalAttackCooltime = unitTemplateData->pod.m_atkCooltime / (1 + adderAttackSpeed);
	float attackDelayMultiplier = 1;
	// 공격 애니메이션 재생시간이 공격주기보다 더 길다면, 애니메이션 재생속도를 더 빠르게 해줘야 한다.
	if (animMinimumTime > finalAttackCooltime)
	{
		attackDelayMultiplier = finalAttackCooltime / animMinimumTime;
		animatorComponent.lock()->GetGI().SetPlaySpeed(animMinimumTime / finalAttackCooltime);
	}
	PlayAnimation(UnitAnimType::Attack);

	switch (unitTemplateData->pod.attackType.enumValue)
	{
		case UnitAttackType::MELEE:
		{
			if (!coroutineAttackEffect.expired())
				FBXPool::Instance().Return(attackVFX);

			coroutineAttackEffect = StartCoroutine(MeleeAttackEffectCoroutine(opponent));
			coroutineAttackEffect.lock()->PushDestroyCallBack([this]()
				{
					FBXPool::Instance().Return(attackVFX);
				});
			break;
		}
	}

	float playSpeed = animatorComponent.lock()->GetGI().GetPlaySpeed();

	co_yield coroutine::WaitForSeconds(unitTemplateData->pod.m_attackPreDelay * attackDelayMultiplier);
	onAttack(opponent);
	playSpeed = animatorComponent.lock()->GetGI().GetPlaySpeed();
	float dmg = unitTemplateData->pod.m_autoAttackDamage + adderAttackDamage;
	DamageType damageType = DamageType::Attack;
	if (math::Random::ByRandomChance(GetCritChance()))
	{
		damageType = DamageType::AttackCrit;
	}
	switch (unitTemplateData->pod.attackType.enumValue)
	{
		case UnitAttackType::MELEE:
		{
			if (!referenceBlindness.BeingReferenced())
			{
				opponent.lock()->Damaged(GetWeakPtr<Unit>(), dmg, damageType);
			}
			else
			{
				ShowMissedUI();
			}
			if (!coroutineAttackEffect.expired())
			{
				attackVFX.lock()->GetGameObject()->GetTransform()->SetWorldPosition(GetTransform()->GetWorldPosition());
				attackVFX.lock()->GetGameObject()->GetTransform()->SetWorldRotation(GetTransform()->GetWorldRotation());
			}
			break;
		}
		case UnitAttackType::MISSILE:
		{
			//auto projectile = ProjectileSelector::SingleInstance().RequestProjectile(GetWeakPtr<Unit>(), opponent.lock(), static_cast<ProjectileType::Enum>(unitTemplateData->pod.projectileType.enumValue));
			auto projectile = ProjectilePool::SingleInstance().Borrow(GetWeakPtr<Unit>(), opponent.lock());
			projectile.lock()->damage = dmg;
			projectile.lock()->SetDamageType(damageType);
			break;
		}
	}

	StartCoroutine(referenceBlockAttack.AcquireForSecondsCoroutine(finalAttackCooltime
		+ math::Random::GetRandomFloat(GetUnitTemplateData().pod.m_atkRandomDelayMin, GetUnitTemplateData().pod.m_atkRandomDelayMax)
		- unitTemplateData->pod.m_attackPreDelay * attackDelayMultiplier));
	auto blockCommand = referenceBlockPendingOrder.Acquire();
	if (unitTemplateData->pod.skinnedFBXName == "SKM_HeartQueen")
	{
		wanderResources::GetAnimation(unitTemplateData->pod.skinnedFBXName, UnitAnimType::AttackToIdle)->SetLoop(false);
		PlayAnimation(UnitAnimType::AttackToIdle);
		co_yield coroutine::WaitForSeconds(wanderResources::GetAnimation(unitTemplateData->pod.skinnedFBXName, UnitAnimType::AttackToIdle)->GetDuration());
	}
	else
	{
		co_yield coroutine::WaitForSeconds(unitTemplateData->pod.m_attackPostDelay * attackDelayMultiplier);
	}
	playSpeed = animatorComponent.lock()->GetGI().GetPlaySpeed();
	blockCommand.reset();
	co_return;
}
yunutyEngine::coroutine::Coroutine Unit::MeleeAttackEffectCoroutine(std::weak_ptr<Unit> opponent)
{
	/// VFX 실행
	attackVFX = wanderResources::GetVFX(unitTemplateData->pod.skinnedFBXName, UnitAnimType::Attack);

	co_await std::suspend_always{};

	auto vfxAnimator = attackVFX.lock()->AcquireVFXAnimator();
	vfxAnimator.lock()->SetAutoActiveFalse();
	vfxAnimator.lock()->Init();
	vfxAnimator.lock()->Play();
	Vector3d startPos = GetTransform()->GetWorldPosition();
	Vector3d deltaPos = opponent.lock()->GetTransform()->GetWorldPosition() - GetTransform()->GetWorldPosition();
	Vector3d direction = deltaPos.Normalized();
	attackVFX.lock()->GetGameObject()->GetTransform()->SetWorldPosition(startPos);
	attackVFX.lock()->GetGameObject()->GetTransform()->SetWorldRotation(GetTransform()->GetWorldRotation());
	attackVFX.lock()->GetGameObject()->GetTransform()->SetWorldScale(Vector3d::one * unitTemplateData->pod.unit_scale);

	while (!vfxAnimator.lock()->IsDone())
	{
		co_await std::suspend_always{};
	}

	co_return;
}
yunutyEngine::coroutine::Coroutine Unit::DmgIndicatorCoroutine(float dmg, UIEnumID uiId)
{
	const auto& gc = GlobalConstant::GetSingletonInstance().pod;
	auto dmgIndicator{ dmgIndicators.at(dmgIndicatorIdx = (dmgIndicatorIdx + 1) % gc.maxDmgIndicatorCount) };
	if (dmgIndicator.expired() || dmgIndicator.lock()->GetActive() == false)
	{
		dmgIndicator = dmgIndicators[dmgIndicatorIdx] = DuplicatedUIPool::Instance().Borrow(uiId);
	}
	Vector3d dmgIndicatorPosition = GetTransform()->GetWorldPosition();
	auto element = dmgIndicator.lock()->GetUIElement();
	element->GetLocalUIsByEnumID().at(UIEnumID::DamageIndicator_Number)->SetNumber(dmg);
	auto uiPos = UIManager::Instance().GetUIPosFromWorld(dmgIndicatorPosition);
	auto idealScale = Vector3d::one * gc.dmgToScaleMultiplier * std::powf(dmg, gc.dmgToScalePower);
	element->GetTransform()->SetWorldPosition(uiPos);
	element->GetTransform()->SetLocalScale(idealScale);
	element->EnableElement();
	float randomDirection = math::Random::GetRandomFloat(-math::PI, math::PI);
	Vector2d randomDirVector = Vector2d{ cos(randomDirection),sin(randomDirection) };
	Vector2d uiSpaceOffsetStart{ unitTemplateData->pod.dmgIndicator_offset.x,unitTemplateData->pod.dmgIndicator_offset.y };
	uiSpaceOffsetStart += randomDirVector * math::Random::GetRandomFloat(unitTemplateData->pod.dmgIndicator_offsetNoiseMin, unitTemplateData->pod.dmgIndicator_offsetNoiseMax);
	Vector2d uiSpaceOffsetEnd = uiSpaceOffsetStart + randomDirVector * math::Random::GetRandomFloat(unitTemplateData->pod.dmgIndicator_travelNoiseMin, unitTemplateData->pod.dmgIndicator_travelNoiseMax);
	//Vector2d{ unitTemplateData->pod.dmgIndicator_offset,0 };

	coroutine::ForSeconds forSeconds{ 1,true };
	while ((!dmgIndicator.expired() && dmgIndicator.lock()->GetActive() == true) && forSeconds.Tick())
	{
		float scaleAlpha = std::sinf(std::fminf(1, forSeconds.Elapsed() / gc.dmgIndicatorPulseEnlargingDuration) * math::PI);
		element->GetTransform()->SetLocalScale(Vector3d::Lerp(idealScale, idealScale * gc.dmgIndicatorPulseEnlargingScale, scaleAlpha));
		dmgIndicator.lock()->GetTransform()->SetWorldPosition(UIManager::Instance().GetUIPosFromWorld(dmgIndicatorPosition)
			+ Vector3d::Lerp(uiSpaceOffsetStart, uiSpaceOffsetEnd, std::sinf(forSeconds.ElapsedNormalized() * 0.5f * math::PI)));
		co_await std::suspend_always{};
	}
	while (!dmgIndicator.expired() && dmgIndicator.lock()->GetActive() == true)
	{
		dmgIndicator.lock()->GetTransform()->SetWorldPosition(UIManager::Instance().GetUIPosFromWorld(dmgIndicatorPosition) + uiSpaceOffsetEnd);
		co_await std::suspend_always{};
	}
	co_return;
}
void Unit::ShowMissedUI()
{
	auto missedIndicator = DuplicatedUIPool::Instance().Borrow(UIEnumID::DamageIndicator_Missed);
	missedIndicator.lock()->GetUIElement()->SetAsWorldSpaceUI(GetTransform()->GetWorldPosition());
}
void Unit::UpdateAttackTargetWithinRange()
{
	auto currentTarget = currentTargetUnit.lock();
	// 현재 타깃이 유효하면 그냥 리턴
	if (currentTarget && currentTarget->IsAlive() && !currentTarget->IsInvulenerable() && currentTarget->GetActive())
	{
		return;
	}
	else
	{
		currentTargetUnit.reset();
		currentTargetUnit = GetClosestEnemyWithinAttackRange();
		return;
	}
}
float Unit::DistanceTo(const Vector3d& target)
{
	return (GetTransform()->GetWorldPosition() - target).Magnitude();
}
void Unit::ReturnToPool()
{
	for (auto unitStatusUI : unitStatusUIs)
	{
		if (!unitStatusUI.expired() && (unitStatusUI.lock()->runtimeFlags & UnitStatusBarFlag::DestroyOnDeath))
		{
			Scene::getCurrentScene()->DestroyGameObject(unitStatusUI.lock()->GetGameObject());
			unitStatusUI.reset();
		}
	}
	unitStatusUIs.clear();
	navAgentComponent.lock()->SetActive(false);
	navObstacle.lock()->SetActive(false);
	unitStatusPortraitUI.reset();
	unitStatusPortraitUI2.reset();
	Reset();
	if (unitData)
	{
		if (!unitData->inGameUnit.expired())
		{
			unitData->inGameUnit.reset();
		}
		unitData = nullptr;
	}
	UnitPool::SingleInstance().Return(GetWeakPtr<Unit>());
}
void Unit::SetSkinnedMeshRenderer(GameObject* fbxObj)
{
	for (auto each : fbxObj->GetChildren())
	{
		if (this->skinnedMeshRenderer = each->GetComponent<graphics::SkinnedMesh>())
		{
			return;
		}
	}
}
void Unit::ResetSharedRef()
{
	enableNavObstacleByState.reset();
	blockFollowingNavAgentByState.reset();
	disableNavAgentByState.reset();
	invulnerabilityByState.reset();
}
void Unit::ResetCallbacks()
{
	onAttack.Clear();
	onAttackHit.Clear();
	onDamaged.Clear();
	onCreated.Clear();
	onRotationFinish.Clear();
	onSkillActivation.Clear();
	onSkillExpiration.Clear();
	for (auto& each : onStateEngage)
	{
		each.Clear();
	}
	for (auto& each : onStateExit)
	{
		each.Clear();
	}
}

const editor::Unit_TemplateData& Unit::GetUnitTemplateData()const
{
	return *unitTemplateData;
};
int Unit::GetTeamIndex() const
{
	return teamIndex;
}
std::weak_ptr<Unit> Unit::GetAttackTarget() const
{
	if (currentTargetUnit.expired())
		return pendingTargetUnit;
	return currentTargetUnit;
}
