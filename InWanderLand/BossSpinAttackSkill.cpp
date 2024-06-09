#include "InWanderLand.h"
#include "BossSpinAttackSkill.h"
#include "VFXAnimator.h"

POD_BossSpinAttackSkill BossSpinAttackSkill::pod = POD_BossSpinAttackSkill();

const float totalTime = 5.2f;
const float spinEndTime = 3.15f;
const float spinStartTime = 2.07f;
const float spinAttackingTime = spinEndTime - spinStartTime;
const float afterSpinDelay = totalTime - spinStartTime - spinAttackingTime;

coroutine::Coroutine BossSpinAttackSkill::SpawningSkillffect()
{
	Vector3d startPos = owner.lock()->GetTransform()->GetWorldPosition();
	Vector3d deltaPos = targetPos - owner.lock()->GetTransform()->GetWorldPosition();
	Vector3d direction = deltaPos.Normalized();

	auto chargeEffect = FBXPool::SingleInstance().Borrow("VFX_HeartQueen_Skill1");

	chargeEffect.lock()->GetGameObject()->GetTransform()->SetWorldPosition(startPos);
	chargeEffect.lock()->GetGameObject()->GetTransform()->SetWorldRotation(Quaternion::MakeWithForwardUp(direction, direction.up));
	auto chargeEffectAnimator = chargeEffect.lock()->AcquireVFXAnimator();
	chargeEffectAnimator.lock()->SetAutoActiveFalse();
	chargeEffectAnimator.lock()->Init();

	co_await std::suspend_always{};

	while (!chargeEffectAnimator.lock()->IsDone())
	{
		co_await std::suspend_always{};
	}

	FBXPool::SingleInstance().Return(chargeEffect);

	co_return;
}

coroutine::Coroutine BossSpinAttackSkill::operator()()
{
    auto blockFollowingNavigation = owner.lock()->referenceBlockFollowingNavAgent.Acquire();
    auto blockAnimLoop = owner.lock()->referenceBlockAnimLoop.Acquire();
    auto disableNavAgent = owner.lock()->referenceDisableNavAgent.Acquire();

    owner.lock()->PlayAnimation(UnitAnimType::Skill1, true);
    owner.lock()->StartCoroutine(SpawningSkillffect());

    knockbackCollider = UnitAcquisitionSphereColliderPool::SingleInstance().Borrow(owner.lock());
    knockbackCollider.lock()->SetRadius(pod.colliderRadius);
    knockbackCollider.lock()->GetTransform()->SetWorldPosition(owner.lock()->GetTransform()->GetWorldPosition());

    co_yield coroutine::WaitForSeconds(spinStartTime);

    coroutine::ForSeconds forSeconds{ spinAttackingTime };

    while (forSeconds.Tick())
    {
        co_await std::suspend_always{};
        for (auto& each : knockbackCollider.lock()->GetEnemies())
        {
            if (knockBackList.contains(each))
            {
                continue;
            }
            knockBackList.insert(each);
            Vector3d delta = pod.knockBackDistance * (each->GetTransform()->GetWorldPosition() - owner.lock()->GetTransform()->GetWorldPosition()).Normalized();
            each->Damaged(owner, pod.skillDamage);
            each->Paralyze(pod.paralyzingDuration);
            each->KnockBack(owner.lock()->GetTransform()->GetWorldPosition() + delta, pod.knockBackDuration);
        }
    }
    disableNavAgent.reset();
    blockFollowingNavigation.reset();
    owner.lock()->Relocate(owner.lock()->GetTransform()->GetWorldPosition());
    co_yield coroutine::WaitForSeconds(afterSpinDelay);
    owner.lock()->PlayAnimation(UnitAnimType::Idle, true);
    co_yield coroutine::WaitForSeconds(0.2);
    OnInterruption();
    co_return;
}

void BossSpinAttackSkill::OnInterruption()
{
    knockbackCollider.lock()->SetRadius(0.5);
    UnitAcquisitionSphereColliderPool::SingleInstance().Return(knockbackCollider);
    knockBackList.clear();
}
