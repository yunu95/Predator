#include "InWanderLand.h"
#include "BossImpaleSkill.h"
#include "VFXAnimator.h"

POD_BossImpaleSkill BossImpaleSkill::pod = POD_BossImpaleSkill();

std::queue<std::weak_ptr<UnitAcquisitionSphereCollider>> BossImpaleSkill::knockbackColliderQueue = std::queue<std::weak_ptr<UnitAcquisitionSphereCollider>>();
std::queue<std::weak_ptr<ManagedFBX>> BossImpaleSkill::spearFbxQueue = std::queue<std::weak_ptr<ManagedFBX>>();

const float impaleStartTime = 3.10f;

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
	float ovalHeight = BossImpaleSkill::pod.impaleSkillRange * 0.5f;
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
	std::for_each(spears.begin(), spears.end(), [=](BossSpear& a) { a.position.y += BossImpaleSkill::pod.impaleSkillRange * 0.5f; });
	std::sort(spears.begin(), spears.end(), [](const BossSpear& a, const BossSpear& b) { return a.timeOffset < b.timeOffset; });
	return spears;
}

coroutine::Coroutine BossImpaleSkill::operator()()
{
	//auto blockFollowingNavigation = owner.lock()->referenceBlockFollowingNavAgent.Acquire();
	auto blockAnimLoop = owner.lock()->referenceBlockAnimLoop.Acquire();
	auto disableNavAgent = owner.lock()->referenceDisableNavAgent.Acquire();
	auto enableNavObstacle = owner.lock()->referenceEnableNavObstacle.Acquire();
	auto rotRef = owner.lock()->referenceBlockRotation.Acquire();
	animator = owner.lock()->GetAnimator();
	auto impaleAnim = wanderResources::GetAnimation(owner.lock()->GetFBXName(), UnitAnimType::Skill2);

	// 창이 생성되는 시간 오프셋은 유닛으로부터의 거리와 정비례한다.
	owner.lock()->SetDefaultAnimation(UnitAnimType::None);
	owner.lock()->PlayAnimation(UnitAnimType::Skill2);

	effectCoroutine = owner.lock()->StartCoroutine(SpawningSkillffect(std::dynamic_pointer_cast<BossImpaleSkill>(selfWeakPtr.lock())));
	effectCoroutine.lock()->PushDestroyCallBack([this]()
		{
			animator.lock()->GetGI().SetPlaySpeed(1);
			previewEffectAnimator.lock()->SetSpeed(1);
			impaleEffectAnimator.lock()->SetSpeed(1);
			FBXPool::Instance().Return(impaleEffect);
			FBXPool::Instance().Return(previewEffect);
		});

	float localForeswingDuration;
	if (pod.foreswingDuration > 0)
	{
		foreswingSpeed = impaleStartTime / pod.foreswingDuration;
		localForeswingDuration = pod.foreswingDuration;
	}
	else
	{
		localForeswingDuration = impaleStartTime;
	}

	wanderUtils::UnitCoroutine::ForSecondsFromUnit waitImpaleStart{ owner, localForeswingDuration };

	while (waitImpaleStart.Tick())
	{
		co_await std::suspend_always();
	}

	wanderUtils::UnitCoroutine::ForSecondsFromUnit waitImpaleDuration{ owner, pod.impaleSkillDuration };
	auto spearVec = BossSpearsInfo();

	for (auto& each : spearVec)
	{
		while (each.timeOffset > waitImpaleDuration.Elapsed())
		{
			waitImpaleDuration.Tick();
			co_await std::suspend_always{};
		}

		auto spearAriseCoroutine = ContentsCoroutine::StartRoutine(SpearArise(std::dynamic_pointer_cast<BossImpaleSkill>(selfWeakPtr.lock()), each.position));
		spearAriseCoroutine.lock()->PushDestroyCallBack([this]()
			{
				if (!spearFbxQueue.empty())
				{
					if (!spearFbxQueue.front().expired())
					{
						FBXPool::Instance().Return(spearFbxQueue.front());
						spearFbxQueue.pop();
					}
				}

				if (!knockbackColliderQueue.empty())
				{
					if (!knockbackColliderQueue.front().expired())
					{
						UnitAcquisitionSphereColliderPool::Instance().Return(knockbackColliderQueue.front());
						knockbackColliderQueue.pop();
					}
				}
			});
	}

	float localBackswingDuration;
	if (pod.backswingDuration > 0)
	{
		backswingSpeed = (wanderResources::GetAnimation(owner.lock()->GetUnitTemplateData().pod.skinnedFBXName, UnitAnimType::Skill2)->GetDuration() - impaleStartTime) / pod.backswingDuration;
		localBackswingDuration = pod.backswingDuration;
	}
	else
	{
		localBackswingDuration = wanderResources::GetAnimation(owner.lock()->GetUnitTemplateData().pod.skinnedFBXName, UnitAnimType::Skill2)->GetDuration() - localForeswingDuration;
	}

	wanderUtils::UnitCoroutine::ForSecondsFromUnit waitImpaleAfter{ owner, localBackswingDuration /*- waitImpaleDuration.Elapsed()*/ };

	while (waitImpaleAfter.Tick())
	{
		co_await std::suspend_always();
	}


	if (owner.lock()->GetPendingOrderType() == UnitOrderType::None)
		owner.lock()->OrderAttackMove();
	animator.lock()->GetGI().SetPlaySpeed(1);

	co_return;
}

void BossImpaleSkill::OnInterruption()
{
	if (!effectCoroutine.expired())
	{
		owner.lock()->DeleteCoroutine(effectCoroutine);
	}
}

void BossImpaleSkill::OnPause()
{
	if (!impaleEffectAnimator.expired())
	{
		impaleEffectAnimator.lock()->Pause();
	}

	if (!previewEffectAnimator.expired())
	{
		previewEffectAnimator.lock()->Pause();
	}
}

void BossImpaleSkill::OnResume()
{
	if (!impaleEffectAnimator.expired())
	{
		impaleEffectAnimator.lock()->Resume();
	}

	if (!previewEffectAnimator.expired())
	{
		previewEffectAnimator.lock()->Resume();
	}
}

// 창이 한번 불쑥 튀어나왔다가 다시 꺼지는 사이클
coroutine::Coroutine BossImpaleSkill::SpearArise(std::weak_ptr<BossImpaleSkill> skill, Vector2d pos)
{
	auto persistance = skill.lock();
	auto fbx = FBXPool::Instance().Borrow(wanderResources::GetFBXName(wanderResources::WanderFBX::IMPALING_SPIKE));
	spearFbxQueue.push(fbx);
	auto collider = UnitAcquisitionSphereColliderPool::Instance().Borrow(persistance->owner);
	knockbackColliderQueue.push(collider);
	auto forward = owner.lock()->GetTransform()->GetWorldRotation().Forward();
	auto right = owner.lock()->GetTransform()->GetWorldRotation().Right();
	auto worldPos = owner.lock()->GetTransform()->GetWorldPosition() + forward * (pos.y + BossImpaleSkill::pod.impaleSkillFirstSpearOffset) + right * pos.x;
	fbx.lock()->GetTransform()->SetWorldPosition(worldPos);
	collider.lock()->SetRadius(0.01);
	collider.lock()->GetTransform()->SetWorldPosition(worldPos);
	co_await std::suspend_always{};

	wanderUtils::UnitCoroutine::ForSecondsFromUnit waitPerSpear{ persistance->owner, pod.impaleSkillDurationPerSpear };

	while (waitPerSpear.Tick())
	{
		float heightAlpha = std::sinf(waitPerSpear.ElapsedNormalized() * math::PI);
		float yDelta = math::LerpF(pod.impaleSkillMinHeightPerSpear, pod.impaleSkillMaxHeightPerSpear, heightAlpha);
		fbx.lock()->GetTransform()->SetWorldPosition(worldPos + Vector3d::up * yDelta);

		for (auto& each : collider.lock()->GetEnemies())
		{
			if (persistance->damagedUnits.contains(each))
			{
				continue;
			}
			persistance->damagedUnits.insert(each);
			each->Damaged(persistance->owner, pod.impaleSkillDamage);
			Vector3d knockBackDest = each->GetTransform()->GetWorldPosition() + (each->GetTransform()->GetWorldPosition() - worldPos).Normalized() * pod.impaleSkillKnockbackDistance;
			each->KnockBack(knockBackDest, pod.impaleSkillKnockbackDuration);
		}

		co_await std::suspend_always{};
	}

	co_return;
}

coroutine::Coroutine BossImpaleSkill::SpawningSkillffect(std::weak_ptr<BossImpaleSkill> skill)
{
	const float colliderEffectRatio = 13.5f;

	auto persistance = skill.lock();

	Vector3d startPos = owner.lock()->GetTransform()->GetWorldPosition();
	Vector3d deltaPos = targetPos - owner.lock()->GetTransform()->GetWorldPosition();
	Vector3d direction = deltaPos.Normalized();

	previewEffect = FBXPool::Instance().Borrow("VFX_HeartQueen_Skill2_Preview");

	previewEffect.lock()->GetGameObject()->GetTransform()->SetWorldPosition(startPos + owner.lock()->GetTransform()->GetWorldRotation().Forward() * pod.impaleSkillRange / 2 + Vector3d(0.0f, 0.2f, 0.0f) + owner.lock()->GetTransform()->GetWorldRotation().Forward() * pod.impaleSkillFirstSpearOffset);
	previewEffect.lock()->GetGameObject()->GetTransform()->SetWorldRotation(owner.lock()->GetTransform()->GetWorldRotation());
	previewEffect.lock()->GetGameObject()->GetTransform()->SetWorldScale(Vector3d(pod.impaleSkillWidth / colliderEffectRatio,
		1,
		pod.impaleSkillRange / colliderEffectRatio));

	previewEffectAnimator = previewEffect.lock()->AcquireVFXAnimator();
	previewEffectAnimator.lock()->SetAutoActiveFalse();
	previewEffectAnimator.lock()->Init();
	previewEffectAnimator.lock()->Play();

	impaleEffect = FBXPool::Instance().Borrow("VFX_HeartQueen_Skill2");

	impaleEffect.lock()->GetGameObject()->GetTransform()->SetWorldPosition(startPos);
	impaleEffect.lock()->GetGameObject()->GetTransform()->SetWorldRotation(owner.lock()->GetTransform()->GetWorldRotation());
	impaleEffect.lock()->GetGameObject()->GetTransform()->SetWorldScale(Vector3d::one * owner.lock()->GetUnitTemplateData().pod.unit_scale);

	impaleEffectAnimator = impaleEffect.lock()->AcquireVFXAnimator();
	impaleEffectAnimator.lock()->SetAutoActiveFalse();
	impaleEffectAnimator.lock()->Init();
	impaleEffectAnimator.lock()->Play();

	float localForeswingDuration;
	if (pod.foreswingDuration > 0)
	{
		foreswingSpeed = impaleStartTime / pod.foreswingDuration;
		localForeswingDuration = pod.foreswingDuration;
	}
	else
	{
		localForeswingDuration = impaleStartTime;
	}
	animator.lock()->GetGI().SetPlaySpeed(foreswingSpeed);
	previewEffectAnimator.lock()->SetSpeed(foreswingSpeed);
	impaleEffectAnimator.lock()->SetSpeed(foreswingSpeed);

	wanderUtils::UnitCoroutine::ForSecondsFromUnit waitImpaleStart{ owner, localForeswingDuration };

	while (waitImpaleStart.Tick())
	{
		co_await std::suspend_always();
	}

	float localBackswingDuration;
	if (pod.backswingDuration > 0)
	{
		backswingSpeed = (wanderResources::GetAnimation(owner.lock()->GetUnitTemplateData().pod.skinnedFBXName, UnitAnimType::Skill2)->GetDuration() - impaleStartTime) / pod.backswingDuration;
		localBackswingDuration = pod.backswingDuration;
	}
	else
	{
		localBackswingDuration = wanderResources::GetAnimation(owner.lock()->GetUnitTemplateData().pod.skinnedFBXName, UnitAnimType::Skill2)->GetDuration() - localForeswingDuration;
	}
	animator.lock()->GetGI().SetPlaySpeed(backswingSpeed);
	impaleEffectAnimator.lock()->SetSpeed(backswingSpeed);

	wanderUtils::UnitCoroutine::ForSecondsFromUnit waitImpaleEnd{ owner, localBackswingDuration };

	while (waitImpaleEnd.Tick())
	{
		co_await std::suspend_always();
	}

	co_return;
}
