#include "InWanderLand.h"
#include "HanselProjectileSkill.h"

coroutine::Coroutine HanselProjectileSkill::operator()()
{
    const application::POD_GlobalConstant& gc = GlobalConstant::GetSingletonInstance().pod;

    Vector3d startPos = owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d deltaPos = targetPos - owner.lock()->GetTransform()->GetWorldPosition();
    Vector3d direction = deltaPos.Normalized();
    owner.lock()->SetDesiredRotation(direction);
    Vector3d endPos = startPos + deltaPos;
    Vector3d currentPos = startPos;

    owner.lock()->PlayAnimation(UnitAnimType::Skill2, true);

    co_return;
}

void HanselProjectileSkill::OnInterruption()
{
}
