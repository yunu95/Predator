#include "InWanderLand.h"
#include "BossImpaleSkill.h"
#include "VFXAnimator.h"

POD_BossImpaleSkill BossImpaleSkill::pod = POD_BossImpaleSkill();

const float impaleStartTime = 3.02f;

struct BossSpear
{
	Vector2d position;
	float timeOffset;
};
const std::vector<BossSpear> BossSpearsInfo()
{
	std::vector<BossSpear> spears;
	// 타원에 빽뺵하게 창을 생성
	// y는 캐릭터 기준 전방방향, x는 우측방향
	float ovalHeight = (BossImpaleSkill::pod.impaleSkillRange - BossImpaleSkill::pod.impaleSkillFirstSpearOffset) * 0.5f;
	float ovalWidth = BossImpaleSkill::pod.impaleSkillWidth * 0.5;
	float ovalHeightSqr = ovalHeight * ovalHeight;
	float ovalWidthSqr = ovalWidth * ovalWidth;

	float currX{ 0 };
	float currY{ -ovalHeight };
	float currOvalWidth = 0;
	while (currY < ovalHeight)
	{
		const float& noise = BossImpaleSkill::pod.impaleSkillAriseDistanceNoisePerSpear;
		float x = { currX + math::Random::GetRandomFloat(noise) };
		float y = { currY + math::Random::GetRandomFloat(noise) };
		if (x >= currOvalWidth)
		{
			currY += BossImpaleSkill::pod.impaleSkillAriseDistancePerSpear;
			currOvalWidth = ovalWidth * sqrtf(1 - currY * currY / ovalHeightSqr);
			currX = -currOvalWidth;
		}
		else
		{
			currX += BossImpaleSkill::pod.impaleSkillAriseDistancePerSpear;
			if (currX >= -currOvalWidth)
			{
				spears.push_back({ { x, y}, 0 });
				spears.rbegin()->timeOffset = math::Random::GetRandomFloat(0, BossImpaleSkill::pod.impaleSkillAriseTimeNoisePerSpear);
			}
		}
	}
	std::for_each(spears.begin(), spears.end(), [=](BossSpear& a) { a.position.y += BossImpaleSkill::pod.impaleSkillFirstSpearOffset + ovalHeight; });
	std::sort(spears.begin(), spears.end(), [](const BossSpear& a, const BossSpear& b) { return a.timeOffset < b.timeOffset; });
	return spears;
}

coroutine::Coroutine BossImpaleSkill::operator()()
{
	auto blockAnimLoop = owner.lock()->referenceBlockAnimLoop.Acquire();
	auto disableNavAgent = owner.lock()->referenceDisableNavAgent.Acquire();
	auto enableNavObstacle = owner.lock()->referenceEnableNavObstacle.Acquire();

	effectCoroutine = owner.lock()->StartCoroutine(SpawningSkillffect(dynamic_pointer_cast<BossImpaleSkill>(selfWeakPtr.lock())));
	effectCoroutine.lock()->PushDestroyCallBack([this]()
		{
			FBXPool::SingleInstance().Return(impaleEffect);
		});

	co_yield coroutine::WaitForSeconds(0.7f);

	auto animator = owner.lock()->GetAnimator();
	auto impaleAnim = wanderResources::GetAnimation(owner.lock()->GetFBXName(), UnitAnimType::Skill2);

	owner.lock()->PlayAnimation(UnitAnimType::Skill2);

	co_yield coroutine::WaitForSeconds{ impaleStartTime };

	coroutine::ForSeconds forSeconds{ pod.impaleSkillDuration };
	managingIndex = 0;

	for (auto& each : BossSpearsInfo())
	{
		while (each.timeOffset > forSeconds.Elapsed())
		{
			forSeconds.Tick();
			co_await std::suspend_always{};
		}

		std::weak_ptr<ManagedFBX> fbx;
		std::weak_ptr<UnitAcquisitionSphereCollider> collider;

		auto spearAriseCoroutine = owner.lock()->StartCoroutine(SpearArise(std::dynamic_pointer_cast<BossImpaleSkill>(selfWeakPtr.lock()), fbx, collider, each.position));
		spearAriseCoroutine.lock()->PushDestroyCallBack([this]()
			{
				if (knockbackColliderVector.empty() && spearFbxVector.empty())
					return ;
				UnitAcquisitionSphereColliderPool::Instance().Return(knockbackColliderVector[managingIndex]);
				FBXPool::SingleInstance().Return(spearFbxVector[managingIndex]);
				managingIndex++;
			});
	}
	
	co_yield coroutine::WaitForSeconds{ 2.0f };

	owner.lock()->PlayAnimation(UnitAnimType::Idle);
	co_return;
}

void BossImpaleSkill::OnInterruption()
{
	if (!effectCoroutine.expired())
	{
		FBXPool::SingleInstance().Return(impaleEffect);
	}
}

// 창이 한번 불쑥 튀어나왔다가 다시 꺼지는 사이클
coroutine::Coroutine BossImpaleSkill::SpearArise(std::weak_ptr<BossImpaleSkill> skill, std::weak_ptr<ManagedFBX> fbx, std::weak_ptr<UnitAcquisitionSphereCollider> collider, Vector2d pos)
{
	auto temp = skill.lock();
	fbx = FBXPool::SingleInstance().Borrow(wanderResources::GetFBXName(wanderResources::WanderFBX::IMPALING_SPIKE));
	skill.lock()->spearFbxVector.push_back(fbx);
	collider = UnitAcquisitionSphereColliderPool::Instance().Borrow(skill.lock()->owner);
	skill.lock()->knockbackColliderVector.push_back(collider);
	auto forward = owner.lock()->GetTransform()->GetWorldRotation().Forward();
	auto right = owner.lock()->GetTransform()->GetWorldRotation().Right();
	auto worldPos = owner.lock()->GetTransform()->GetWorldPosition() + forward * pos.y + right * pos.x;
	fbx.lock()->GetTransform()->SetWorldPosition(worldPos);
	collider.lock()->SetRadius(0.01);
	collider.lock()->GetTransform()->SetWorldPosition(worldPos);
	co_await std::suspend_always{};
	if (!skill.expired())
	{
		skill.lock();
		for (auto& each : collider.lock()->GetEnemies())
		{
			if (skill.lock()->damagedUnits.contains(each))
			{
				continue;
			}
			skill.lock()->damagedUnits.insert(each);
			each->Damaged(skill.lock()->owner, pod.impaleSkillDamage);
			Vector3d knockBackDest = each->GetTransform()->GetWorldPosition() + (each->GetTransform()->GetWorldPosition() - worldPos).Normalized() * pod.impaleSkillKnockbackDistance;
			each->KnockBack(knockBackDest, pod.impaleSkillKnockbackDuration);
		}
	}

	coroutine::ForSeconds forSeconds{ pod.impaleSkillDurationPerSpear };
	while (forSeconds.Tick())
	{
		skill.lock();
		float heightAlpha = std::sinf(forSeconds.ElapsedNormalized() * math::PI);
		float yDelta = math::LerpF(pod.impaleSkillMinHeightPerSpear, pod.impaleSkillMaxHeightPerSpear, heightAlpha);
		fbx.lock()->GetTransform()->SetWorldPosition(worldPos + Vector3d::up * yDelta);
		co_await std::suspend_always{};
	}

	co_return;
}

coroutine::Coroutine BossImpaleSkill::SpawningSkillffect(std::weak_ptr<BossImpaleSkill> skill)
{
	skill.lock();
	Vector3d startPos = owner.lock()->GetTransform()->GetWorldPosition();
	Vector3d deltaPos = targetPos - owner.lock()->GetTransform()->GetWorldPosition();
	Vector3d direction = deltaPos.Normalized();

	impaleEffect = FBXPool::SingleInstance().Borrow("VFX_HeartQueen_Skill2");

	impaleEffect.lock()->GetGameObject()->GetTransform()->SetWorldPosition(startPos);
	impaleEffect.lock()->GetGameObject()->GetTransform()->SetWorldRotation(Quaternion::MakeWithForwardUp(direction, direction.up));
	impaleEffect.lock()->GetGameObject()->GetTransform()->SetWorldScale(owner.lock()->GetTransform()->GetWorldScale());
	
	auto chargeEffectAnimator = impaleEffect.lock()->AcquireVFXAnimator();
	chargeEffectAnimator.lock()->SetAutoActiveFalse();
	chargeEffectAnimator.lock()->Init();

	co_await std::suspend_always{};

	while (!chargeEffectAnimator.lock()->IsDone())
	{
		co_await std::suspend_always{};
	}

	co_return;
}
