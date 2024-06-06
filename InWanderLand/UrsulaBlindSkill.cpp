#include "InWanderLand.h"
#include "UrsulaBlindSkill.h"

#include <math.h>

Vector3d UrsulaBlindSkill::skillStart = Vector3d();
Vector3d UrsulaBlindSkill::skillDestination = Vector3d();

POD_UrsulaBlindSkill UrsulaBlindSkill::pod = POD_UrsulaBlindSkill();

coroutine::Coroutine UrsulaBlindSkill::operator()()
{
    UpdatePosition(owner.lock()->GetGameObject()->GetTransform()->GetWorldPosition(), targetPos);
    co_await std::suspend_always{};

    owner.lock()->PlayAnimation(UnitAnimType::Skill1, true);
    auto animator = owner.lock()->GetAnimator();
    auto anim = wanderResources::GetAnimation(owner.lock()->name, UnitAnimType::Skill1);
    coroutine::ForSeconds forSeconds{ anim->GetDuration() };
    circle_Top = UnitAcquisitionSphereColliderPool::SingleInstance().Borrow(owner.lock());
    circle_Top.lock()->SetRadius(pod.skillRadius);
    circle_Left = UnitAcquisitionSphereColliderPool::SingleInstance().Borrow(owner.lock());
    circle_Left.lock()->SetRadius(pod.skillRadius);
    circle_Right = UnitAcquisitionSphereColliderPool::SingleInstance().Borrow(owner.lock());
    circle_Right.lock()->SetRadius(pod.skillRadius);

    circle_Top.lock()->GetTransform()->SetWorldPosition(GetSkillObjectPos_Top());
    circle_Left.lock()->GetTransform()->SetWorldPosition(GetSkillObjectPos_Left());
    circle_Right.lock()->GetTransform()->SetWorldPosition(GetSkillObjectPos_Right());

    co_await std::suspend_always{};

    int hitCount = 0;
    while (forSeconds.Tick())
    {
        auto curFrame = animator.lock()->GetCurrentFrame();
        bool hit = false;
        if (hitCount == 0 && (curFrame >= 22 && curFrame < 40))
        {
            hit = true;
            hitCount++;
        }
        else if (hitCount == 1 && (curFrame >= 40 && curFrame < 58))
        {
            hit = true;
            hitCount++;
        }
        else if (hitCount == 2 && curFrame >= 58)
        {
            hit = true;
            hitCount++;
        }

        if (hit)
        {
            co_await std::suspend_always{};
            for (auto& each : circle_Top.lock()->GetEnemies())
            {
                each->Damaged(owner, pod.skillDamage);

                /// 실명
                /// 실명 대상은 skillBlindTime 동안 실명 상태
            }

            for (auto& each : circle_Left.lock()->GetEnemies())
            {
                each->Damaged(owner, pod.skillDamage);

                /// 실명
                /// 실명 대상은 skillBlindTime 동안 실명 상태
            }

            for (auto& each : circle_Right.lock()->GetEnemies())
            {
                each->Damaged(owner, pod.skillDamage);

                /// 실명
                /// 실명 대상은 skillBlindTime 동안 실명 상태
            }
        }

        /// 우선은 여러 영역 겹칠 경우, 중복하여 대미지 계산함
        co_await std::suspend_always{};
    }

    OnInterruption();
    co_return;
}

void UrsulaBlindSkill::OnInterruption()
{
    circle_Top.lock()->SetRadius(0.5);
    circle_Left.lock()->SetRadius(0.5);
    circle_Right.lock()->SetRadius(0.5);
    UnitAcquisitionSphereColliderPool::SingleInstance().Return(circle_Top);
    UnitAcquisitionSphereColliderPool::SingleInstance().Return(circle_Left);
    UnitAcquisitionSphereColliderPool::SingleInstance().Return(circle_Right);
}

void UrsulaBlindSkill::UpdatePosition(const Vector3d& start, const Vector3d& dest)
{
    skillStart = start;
    skillDestination = dest;
}

Vector3d UrsulaBlindSkill::GetSkillObjectPos_Top()
{
    auto length = pod.skillRadius * 2 + pod.skillOffset;
    auto skillDir = (skillDestination - skillStart).Normalized();
    return skillDestination + std::sqrt(3) / 3 * skillDir * length;
}

Vector3d UrsulaBlindSkill::GetSkillObjectPos_Left()
{
    auto length = pod.skillRadius * 2 + pod.skillOffset;
    auto skillDir = (skillDestination - skillStart).Normalized();
    auto left = Vector3d::Cross(skillDir, Vector3d::up).Normalized();
    return skillDestination - std::sqrt(3) / 6 * skillDir * length + left * length / 2;
}

Vector3d UrsulaBlindSkill::GetSkillObjectPos_Right()
{
    auto length = pod.skillRadius * 2 + pod.skillOffset;
    auto skillDir = (skillDestination - skillStart).Normalized();
    auto right = Vector3d::Cross(-skillDir, Vector3d::up).Normalized();
    return skillDestination - std::sqrt(3) / 6 * skillDir * length + right * length / 2;
}
