#include "InWanderLand.h"
#include "BossSpinAttackSkill.h"

POD_BossSpinAttackSkill BossSpinAttackSkill::pod = POD_BossSpinAttackSkill();

coroutine::Coroutine BossSpinAttackSkill::operator()()
{
	owner.lock()->PlayAnimation(UnitAnimType::Skill1, true);
	coroutine::ForSeconds forSeconds{ pod.skillPlayTime };
	knockBackCollider = UnitAcquisitionSphereColliderPool::SingleInstance().Borrow(owner.lock());
	knockBackCollider.lock()->SetRadius(pod.skillRadius);

	while (forSeconds.Tick())
	{
		knockBackCollider.lock()->GetTransform()->SetWorldPosition(owner.lock()->GetTransform()->GetWorldPosition());
		co_await std::suspend_always{};
		for (auto& each : knockBackCollider.lock()->GetEnemies())
		{
			if (knockBackList.contains(each))
			{
				continue;
			}
			knockBackList.insert(each);
			Vector3d delta = pod.knockBackPower * (each->GetTransform()->GetWorldPosition() - owner.lock()->GetTransform()->GetWorldPosition()).Normalized();
			each->KnockBack(each->GetTransform()->GetWorldPosition() + delta, pod.knckBackDuration);
			each->Damaged(owner, pod.skillDamage);
		}
	}
	OnInterruption();
	co_return;
}

void BossSpinAttackSkill::OnInterruption()
{
	knockBackList.clear();
	knockBackCollider.lock()->SetRadius(0.5);
	UnitAcquisitionSphereColliderPool::SingleInstance().Return(knockBackCollider);
}
