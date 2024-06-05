#include "InWanderLand.h"
#include "UrsulaBlindSkill.h"

#include <math.h>

Vector3d UrsulaBlindSkill::lastSkillPos = Vector3d();
Vector3d UrsulaBlindSkill::lastSkillDir = Vector3d();

POD_UrsulaBlindSkill UrsulaBlindSkill::pod = POD_UrsulaBlindSkill();

coroutine::Coroutine UrsulaBlindSkill::operator()()
{
    lastSkillPos = targetPos;
    lastSkillDir = (lastSkillPos - owner.lock()->GetGameObject()->GetTransform()->GetWorldPosition()).Normalized();
    co_await std::suspend_always{};

    owner.lock()->PlayAnimation(UnitAnimType::Skill1, true);
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

    /// Ursula 가 머리 내릴 때 수행해야 함

    while (forSeconds.Tick())
    {
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

Vector3d UrsulaBlindSkill::GetSkillObjectPos_Top()
{
    auto length = pod.skillRadius * 2 + pod.skillOffset;
    return lastSkillPos + std::sqrt(3) / 3 * lastSkillDir * length;
}

Vector3d UrsulaBlindSkill::GetSkillObjectPos_Left()
{
    auto length = pod.skillRadius * 2 + pod.skillOffset;
    auto left = Vector3d::Cross(lastSkillDir, Vector3d::up).Normalized();
    return lastSkillPos - std::sqrt(3) / 6 * lastSkillDir * length + left * length / 2;
}

Vector3d UrsulaBlindSkill::GetSkillObjectPos_Right()
{
    auto length = pod.skillRadius * 2 + pod.skillOffset;
    auto right = Vector3d::Cross(-lastSkillDir, Vector3d::up).Normalized();
    return lastSkillPos - std::sqrt(3) / 6 * lastSkillDir * length + right * length / 2;
}
