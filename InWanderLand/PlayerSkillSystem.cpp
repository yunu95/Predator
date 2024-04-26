#include "InWanderLand.h"
#include "PlayerSkillSystem.h"
#include "Unit.h"
#include "ContentsLayer.h"
#include "Application.h"

void PlayerSkillSystem::ActivateSkill(Unit::SkillEnum p_currentSkill, Vector3d p_skillPosition)
{
	switch (p_currentSkill)
	{
		case Unit::SkillEnum::Q:
			ActivateSkillOne(p_skillPosition);
			break;
		case Unit::SkillEnum::W:
			ActivateSkillTwo(p_skillPosition);
			break;
		//case Unit::SkillEnum::BossSkillOne:
		//	ActivateSkillTwo(p_skillPosition);
		//	break;
		//case Unit::SkillEnum::BossSkillTwo:
		//	ActivateSkillTwo(p_skillPosition);
		//	break;
		//case Unit::SkillEnum::BossSkillThree:
		//	ActivateSkillTwo(p_skillPosition);
		//	break;
		//case Unit::SkillEnum::BossSkillFour:
		//	ActivateSkillTwo(p_skillPosition);
		//	break;
		default:
			break;
	}
}

void PlayerSkillSystem::RotateProjectile(GameObject* p_projectileObject, Vector3d p_endPosition)
{
	Vector3d startPosition = p_projectileObject->GetTransform()->GetWorldPosition();
	Vector3d objectFront = p_projectileObject->GetTransform()->GetWorldRotation().Forward();
	Vector3d distanceVec = p_endPosition - startPosition;

	double dot = Vector3d::Dot(objectFront, startPosition - p_endPosition);

	double angle;
	double sq;
	double finalAngle;
	double finalDegree;

	// 회전 방향 판정
	Vector3d axis = Vector3d::Cross(objectFront, distanceVec);

	angle = (objectFront.x * distanceVec.x + objectFront.z * distanceVec.z);
	sq = (sqrt(pow(objectFront.x, 2) + pow(objectFront.z, 2)) *
		sqrt(pow(distanceVec.x, 2) + pow(distanceVec.z, 2)));

	// 두 벡터의 각도가 180도 이상이면 180을, -180 이하 이라면 -180을 
	//finalAngle = acos( max( -1.0f, min(1.0f, angle / sq) ) );
	finalAngle = acos(std::clamp(angle / sq, -1.0, 1.0));			// c++17 된다면
	finalDegree = 57.2969f * (finalAngle);

	if (axis.y < 0)
		finalDegree *= -1;

	if (abs(finalDegree) > 0.05)
		p_projectileObject->GetTransform()->SetWorldRotation(Quaternion({ 0.0f, finalDegree, 0.0f }));
}

void PlayerSkillSystem::SetSkillOneRange(float p_rng)
{
	m_skillOneRange = p_rng;
}

void PlayerSkillSystem::SetSkillTwoRange(float p_rng)
{
	m_skillTwoRange = p_rng;
}

void PlayerSkillSystem::SetQSkillCoolTime(float p_coolTime)
{
	qSkillCoolTime = p_coolTime;
}

void PlayerSkillSystem::SetESkillCoolTime(float p_coolTime)
{
	eSkillCoolTime = p_coolTime;
}

float PlayerSkillSystem::GetSkillOneRange() const
{
	return m_skillOneRange;
}

float PlayerSkillSystem::GetSkillTwoRange() const
{
	return m_skillTwoRange;
}

Vector3d PlayerSkillSystem::CheckSkillRange(Vector3d p_skillPos, Unit::SkillEnum p_num)
{
	float tempRng;

	switch (p_num)
	{
		case Unit::SkillEnum::Q:
			tempRng = m_skillOneRange;
			break;
		case Unit::SkillEnum::W:
			tempRng = m_skillTwoRange;
			break;
	}

	float tempDistance = (p_skillPos - GetGameObject()->GetTransform()->GetWorldPosition()).Magnitude();
	if (tempDistance > tempRng)
	{
		tempDistance = tempRng;
		Vector3d finalPosition = GetTransform()->GetWorldPosition() + (p_skillPos - GetTransform()->GetWorldPosition()).Normalized() * tempRng;
		p_skillPos = finalPosition;
	}

	return p_skillPos;
}

void PlayerSkillSystem::Start()
{

}

void PlayerSkillSystem::Update()
{
	if (!isQSkillReady)
	{
		qSkillCoolDownElapsed += Time::GetDeltaTime();
		if (qSkillCoolDownElapsed >= qSkillCoolTime)
		{
			qSkillCoolDownElapsed = 0.0f;
			isQSkillReady = true;
		}
	}
	if (!isESkillReady)
	{
		eSkillCoolDownElapsed += Time::GetDeltaTime();
		if (eSkillCoolDownElapsed >= eSkillCoolTime)
		{
			eSkillCoolDownElapsed = 0.0f;
			isESkillReady = true;
		}
	}
}

bool PlayerSkillSystem::IsSkillCoolingDown(Unit::SkillEnum p_skillnum) const
{

	switch (p_skillnum)
	{
		case Unit::SkillEnum::Q:
			return isQSkillReady;
			break;
		case Unit::SkillEnum::W:
			return isESkillReady;
			break;
		default:
			break;
	}
}
