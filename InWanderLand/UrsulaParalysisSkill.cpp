#include "InWanderLand.h"
#include "UrsulaParalysisSkill.h"
#include "VFXAnimator.h"

#include <math.h>


POD_UrsulaParalysisSkill UrsulaParalysisSkill::pod = POD_UrsulaParalysisSkill();

coroutine::Coroutine UrsulaParalysisSkill::operator()()
{
    auto blockFollowingNavigation = owner.lock()->referenceBlockFollowingNavAgent.Acquire();
    auto blockAnimLoop = owner.lock()->referenceBlockAnimLoop.Acquire();
    auto disableNavAgent = owner.lock()->referenceDisableNavAgent.Acquire();

    owner.lock()->PlayAnimation(UnitAnimType::Skill2, true);
    auto animator = owner.lock()->GetAnimator();
    auto anim = wanderResources::GetAnimation(owner.lock()->GetFBXName(), UnitAnimType::Skill2);
    coroutine::ForSeconds forSeconds{ anim->GetDuration() };
    skillCollider = UnitAcquisitionSphereColliderPool::SingleInstance().Borrow(owner.lock());
    skillCollider.lock()->SetRadius(pod.skillRadius);
    skillCollider.lock()->GetTransform()->SetWorldPosition(targetPos);

    //auto effectParentObject = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
    //auto tentacleObject = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SVFX_Ursula_Skill2_Tentacle");
    //auto waveObject = yunutyEngine::Scene::getCurrentScene()->AddGameObjectFromFBX("SVFX_Ursula_Skill2_Wave");
    auto tentacleObject = FBXPool::SingleInstance().Borrow("SVFX_Ursula_Skill2_Tentacle");
    auto waveObject = FBXPool::SingleInstance().Borrow("SVFX_Ursula_Skill2_Wave");
    tentacleObject.lock()->GetTransform()->SetWorldScale({3,3,3});
    waveObject.lock()->GetTransform()->SetWorldScale({ 3,3,3 });

    auto rsrcManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
    auto& animList = rsrcManager->GetAnimationList();

    yunuGI::IAnimation* tentacleAnimation;
    yunuGI::IAnimation* waveAnimation;

    auto tentacleAnimator = tentacleObject.lock()->GetGameObject()->GetChildren()[0]->GetComponent<yunutyEngine::graphics::Animator>();
    auto waveAnimator = waveObject.lock()->GetGameObject()->GetChildren()[0]->GetComponent<yunutyEngine::graphics::Animator>();

    for (auto each : animList)
    {
        if (each->GetName() == L"root|Ani_SVFX_Tentacle")
        {
            tentacleAnimation = each;
            tentacleAnimator->PushAnimation(tentacleAnimation);
        }
        else if (each->GetName() == L"root|Ani_SVFX_Wave")
        {
            waveAnimation = each;
            waveAnimator->PushAnimation(waveAnimation);
        }
    }

    waveObject.lock()->GetGameObject()->GetChildren()[0]->AddComponent<VFXAnimator>()->SetAutoActiveFalse();

    //tentacleObject.lock()->GetGameObject()->SetParent(effectParentObject);
    //waveObject.lock()->GetGameObject()->SetParent(effectParentObject);

    //effectParentObject->GetTransform()->SetWorldPosition(targetPos);
    tentacleObject.lock()->GetGameObject()->GetTransform()->SetWorldPosition(targetPos + Vector3d(0, 1.0f, 0));
    waveObject.lock()->GetGameObject()->GetTransform()->SetWorldPosition(targetPos + Vector3d(0, 1.0f, 0));

    tentacleAnimator->Play(tentacleAnimation);
    waveAnimator->Play(waveAnimation);

    co_await std::suspend_always{};

    bool hit = false;
    while (!hit)
    {
        auto curFrame = animator.lock()->GetCurrentFrame();

        /// 애니메이션은 몇 번 나눠서 철썩 철썩 느낌인데,
        /// 이펙트가 한 번에 모으는 형태로 기억함
        /// 알아보고 수정 필요
        if (curFrame >= 48)
        {
            hit = true;
        }

        if (hit)
        {
            co_await std::suspend_always{};
            for (auto& each : skillCollider.lock()->GetEnemies())
            {
                each->KnockBack(targetPos, anim->GetDuration() - forSeconds.ElapsedNormalized());
                each->Damaged(owner, pod.skillDamage);
            }
        }
        co_await std::suspend_always{};
    }
    
    while (forSeconds.Tick())
    {
        co_await std::suspend_always{};
    }

    /// 마지막으로 경직
    for (auto& each : skillCollider.lock()->GetEnemies())
    {
        each->Paralyze(pod.skillParalysisTime);
    }

    while (!waveAnimator->IsDone())
    {
        co_await std::suspend_always{};
    }

    FBXPool::SingleInstance().Return(tentacleObject);
    FBXPool::SingleInstance().Return(waveObject);

    disableNavAgent.reset();
    blockFollowingNavigation.reset();
    owner.lock()->Relocate(owner.lock()->GetTransform()->GetWorldPosition());

    OnInterruption();
    co_return;
}

void UrsulaParalysisSkill::OnInterruption()
{
    skillCollider.lock()->SetRadius(0.5);
    UnitAcquisitionSphereColliderPool::SingleInstance().Return(skillCollider);
}
