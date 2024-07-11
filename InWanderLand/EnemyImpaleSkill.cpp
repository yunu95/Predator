#include "InWanderLand.h"
#include "EnemyImpaleSkill.h"
#include "VFXAnimator.h"

POD_EnemyImpaleSkill EnemyImpaleSkill::pod = POD_EnemyImpaleSkill();

std::queue<std::weak_ptr<UnitAcquisitionSphereCollider>> EnemyImpaleSkill::knockbackColliderQueue = std::queue<std::weak_ptr<UnitAcquisitionSphereCollider>>();
std::queue<std::weak_ptr<ManagedFBX>> EnemyImpaleSkill::spearFbxQueue = std::queue<std::weak_ptr<ManagedFBX>>();
const float enemyImpaleStartTime = 1.5f;

struct Spear
{
    Vector2d position;
    float timeOffset;
};
const std::vector<Spear> SpearsInfo()
{
    std::vector<Spear> spears;
    // 타원에 빽뺵하게 창을 생성
    // y는 캐릭터 기준 전방방향, x는 우측방향
    float ovalHeight = EnemyImpaleSkill::pod.impaleSkillRange * 0.5f;
    float ovalWidth = EnemyImpaleSkill::pod.impaleSkillWidth * 0.5;
    float ovalHeightSqr = ovalHeight * ovalHeight;
    float ovalWidthSqr = ovalWidth * ovalWidth;

    float currX{ 0 };
    float currY{ -ovalHeight };
    float currOvalWidth = 0;
    while (currY < ovalHeight)
    {
        const float& noise = EnemyImpaleSkill::pod.impaleSkillAriseDistanceNoisePerSpear;
        float x = { currX + math::Random::GetRandomFloat(noise) };
        float y = { currY + math::Random::GetRandomFloat(noise) };
        if (x >= currOvalWidth)
        {
            currY += EnemyImpaleSkill::pod.impaleSkillAriseDistancePerSpear;
            currOvalWidth = ovalWidth * sqrtf(1 - currY * currY / ovalHeightSqr);
            currX = -currOvalWidth;
        }
        else
        {
            currX += EnemyImpaleSkill::pod.impaleSkillAriseDistancePerSpear;
            if (currX >= -currOvalWidth)
            {
                spears.push_back({ { x, y}, 0 });
                spears.rbegin()->timeOffset = math::Random::GetRandomFloat(0, EnemyImpaleSkill::pod.impaleSkillAriseTimeNoisePerSpear);
            }
        }
    }
    std::for_each(spears.begin(), spears.end(), [=](Spear& a) { a.position.y += EnemyImpaleSkill::pod.impaleSkillRange * 0.5f; });
    std::sort(spears.begin(), spears.end(), [](const Spear& a, const Spear& b) { return a.timeOffset < b.timeOffset; });
    return spears;
}

coroutine::Coroutine EnemyImpaleSkill::operator()()
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

    effectCoroutine = owner.lock()->StartCoroutine(SpawningSkillffect(std::dynamic_pointer_cast<EnemyImpaleSkill>(selfWeakPtr.lock())));
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
        foreswingSpeed = enemyImpaleStartTime / pod.foreswingDuration;
        localForeswingDuration = pod.foreswingDuration;
    }
    else
    {
        localForeswingDuration = enemyImpaleStartTime;
    }

    wanderUtils::UnitCoroutine::ForSecondsFromUnit waitImpaleStart{ owner, localForeswingDuration };

    while (waitImpaleStart.Tick())
    {
        co_await std::suspend_always();
    }

    wanderUtils::UnitCoroutine::ForSecondsFromUnit waitImpaleDuration{ owner, pod.impaleSkillDuration };

    auto spearVec = SpearsInfo();

    for (auto& each : spearVec)
    {
        //while (each.timeOffset > waitImpaleDuration.Elapsed())
        //{
        //    waitImpaleDuration.Tick();
        //    co_await std::suspend_always{};
        //}

        auto spearAriseCoroutine = ContentsCoroutine::StartRoutine(SpearArise(std::dynamic_pointer_cast<EnemyImpaleSkill>(selfWeakPtr.lock()), each.position));
        spearAriseCoroutine.lock()->PushDestroyCallBack([this]()
            {
                if (!spearFbxQueue.empty())
                {
                    if (!spearFbxQueue.front().expired())
                    {
                        spearFbxQueue.front().lock()->GetTransform()->SetWorldRotation(Quaternion::Identity());
                        spearFbxQueue.front().lock()->GetTransform()->SetWorldScale(Vector3d::one);
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
        backswingSpeed = (wanderResources::GetAnimation(owner.lock()->GetUnitTemplateData().pod.skinnedFBXName, UnitAnimType::Skill2)->GetDuration() - enemyImpaleStartTime) / pod.backswingDuration;
        localBackswingDuration = pod.backswingDuration;
    }
    else
    {
        localBackswingDuration = wanderResources::GetAnimation(owner.lock()->GetUnitTemplateData().pod.skinnedFBXName, UnitAnimType::Skill2)->GetDuration() - localForeswingDuration;
    }

    wanderUtils::UnitCoroutine::ForSecondsFromUnit waitImpaleAfter{ owner, localBackswingDuration - waitImpaleDuration.Elapsed() };

    while (waitImpaleAfter.Tick())
    {
        co_await std::suspend_always();
    }


    if (owner.lock()->GetPendingOrderType() == UnitOrderType::None)
        owner.lock()->OrderAttackMove();
    animator.lock()->GetGI().SetPlaySpeed(1);
    co_return;
}

void EnemyImpaleSkill::OnInterruption()
{
    if (!effectCoroutine.expired())
    {
        owner.lock()->DeleteCoroutine(effectCoroutine);
    }
}

void EnemyImpaleSkill::OnPause()
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

void EnemyImpaleSkill::OnResume()
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
coroutine::Coroutine EnemyImpaleSkill::SpearArise(std::weak_ptr<EnemyImpaleSkill> skill, Vector2d pos)
{
    auto persistance = skill.lock();
    auto fbx = FBXPool::Instance().Borrow(wanderResources::GetFBXName(wanderResources::WanderFBX::IMPALING_SPIKE));
    spearFbxQueue.push(fbx);
    auto collider = UnitAcquisitionSphereColliderPool::Instance().Borrow(persistance->owner);
    knockbackColliderQueue.push(collider);
    auto forward = owner.lock()->GetTransform()->GetWorldRotation().Forward();
    auto right = owner.lock()->GetTransform()->GetWorldRotation().Right();
    auto worldPos = owner.lock()->GetTransform()->GetWorldPosition() + forward * (pos.y + EnemyImpaleSkill::pod.impaleSkillFirstSpearOffset) + right * pos.x;
    fbx.lock()->GetTransform()->SetWorldPosition(worldPos);
    collider.lock()->SetRadius(0.01);
    collider.lock()->GetTransform()->SetWorldPosition(worldPos);
    float randRotation = math::Random::GetRandomFloat(pod.minDegree, pod.maxDegree);
    auto randRotAxis = fbx.lock()->GetTransform()->GetWorldRotation().Forward() + Vector3d(0, math::Random::GetRandomFloat(0, 360), 0);
    fbx.lock()->GetTransform()->SetWorldRotation(Vector3d(0, math::Random::GetRandomFloat(0, 360), 0));
    fbx.lock()->GetTransform()->SetWorldRotation(Quaternion::MakeAxisAngleQuaternion(fbx.lock()->GetTransform()->GetWorldRotation().Right(), randRotation * math::Deg2Rad));
    
    float randScale = math::Random::GetRandomFloat(pod.minSpearScale, pod.maxSpearScale);

    fbx.lock()->GetTransform()->SetWorldScale(Vector3d::one * randScale);
    co_await std::suspend_always{};
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

    wanderUtils::UnitCoroutine::ForSecondsFromUnit waitPerSpear{ persistance->owner, pod.impaleSkillDurationPerSpear};

    float localSpearAriseTimeRatio;

    if (pod.spearAriseTimeRatio >= 1)
        localSpearAriseTimeRatio = 0.99f;
    else if (pod.spearAriseTimeRatio <= 0)
        localSpearAriseTimeRatio = 0.01f;
    else
        localSpearAriseTimeRatio = pod.spearAriseTimeRatio;

    while (waitPerSpear.Tick())
    {
        //float heightAlpha = std::sinf(waitPerSpear.ElapsedNormalized() * math::PI);
        float heightAlpha;
        if (waitPerSpear.ElapsedNormalized() <= localSpearAriseTimeRatio)
        {
            heightAlpha = 1 / localSpearAriseTimeRatio * waitPerSpear.ElapsedNormalized();
        }
        else if (waitPerSpear.ElapsedNormalized() >= (1 - localSpearAriseTimeRatio))
        {
            heightAlpha = -1 / localSpearAriseTimeRatio * waitPerSpear.ElapsedNormalized() + 1 + (1 - localSpearAriseTimeRatio) / localSpearAriseTimeRatio;
        }
        else
        {
            heightAlpha = 1;
        }
        float yDelta = math::LerpF(pod.impaleSkillMinHeightPerSpear, pod.impaleSkillMaxHeightPerSpear, heightAlpha);
        fbx.lock()->GetTransform()->SetWorldPosition(worldPos + Vector3d::up * yDelta);
        fbx.lock()->GetTransform()->SetWorldPosition(worldPos + fbx.lock()->GetTransform()->GetWorldRotation().Up() * yDelta * randScale);
        co_await std::suspend_always{};
    }

    co_return;
}

coroutine::Coroutine EnemyImpaleSkill::SpawningSkillffect(std::weak_ptr<EnemyImpaleSkill> skill)
{
    const float colliderEffectRatio = 12.0f;

    auto persistance = skill.lock();

    Vector3d startPos = owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d deltaPos = targetPos - owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d direction = deltaPos.Normalized();

    previewEffect = FBXPool::Instance().Borrow("VFX_Monster2_Skill_Preview");

    previewEffect.lock()->GetGameObject()->GetTransform()->SetWorldPosition(startPos + owner.lock()->GetTransform()->GetWorldRotation().Forward() * pod.impaleSkillRange / 2  + owner.lock()->GetTransform()->GetWorldRotation().Forward() * pod.impaleSkillFirstSpearOffset);
    previewEffect.lock()->GetGameObject()->GetTransform()->SetWorldRotation(owner.lock()->GetTransform()->GetWorldRotation());
    previewEffect.lock()->GetGameObject()->GetTransform()->SetWorldScale(Vector3d(pod.impaleSkillWidth / colliderEffectRatio,
        1,
        pod.impaleSkillRange / colliderEffectRatio));

    previewEffectAnimator = previewEffect.lock()->AcquireVFXAnimator();
    previewEffectAnimator.lock()->SetAutoActiveFalse();
    previewEffectAnimator.lock()->Init();
    previewEffectAnimator.lock()->Play();

    impaleEffect = FBXPool::Instance().Borrow("VFX_Monster2_Skill");

    impaleEffect.lock()->GetGameObject()->GetTransform()->SetWorldPosition(startPos);
    impaleEffect.lock()->GetGameObject()->GetTransform()->SetWorldRotation(owner.lock()->GetTransform()->GetWorldRotation());
    impaleEffect.lock()->GetGameObject()->GetTransform()->SetWorldScale(owner.lock()->GetTransform()->GetWorldScale());

    impaleEffectAnimator = impaleEffect.lock()->AcquireVFXAnimator();
    impaleEffectAnimator.lock()->SetAutoActiveFalse();
    impaleEffectAnimator.lock()->Init();
    impaleEffectAnimator.lock()->Play();

    float localForeswingDuration;
    if (pod.foreswingDuration > 0)
    {
        foreswingSpeed = enemyImpaleStartTime / pod.foreswingDuration;
        localForeswingDuration = pod.foreswingDuration;
    }
    else
    {
        localForeswingDuration = enemyImpaleStartTime;
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
        backswingSpeed = (wanderResources::GetAnimation(owner.lock()->GetUnitTemplateData().pod.skinnedFBXName, UnitAnimType::Skill2)->GetDuration() - enemyImpaleStartTime) / pod.backswingDuration;
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
