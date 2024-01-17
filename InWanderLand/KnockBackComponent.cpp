#include "KnockBackComponent.h"
#include "UnitTransformComponent.h"
#include "Unit.h"
#include "Dotween.h"

void KnockBackComponent::ApplyStatus(Unit* ownerUnit, Unit* opponentUnit)
{
	/// 넉백 시스템
	/// opponent 유닛을 밀어 내야 하는데...
	/// navigationAgent를 끄고 DOMove 후 OnComplete 에 navigationAgent를 붙여준다?

	bool isAlreadyCrushed = false;

	for (auto each : crushedUnitList)
	{
		if (each == opponentUnit)
		{
			isAlreadyCrushed = true;
			break;
		}
	}

	if (!isAlreadyCrushed)
	{
		crushedUnitList.push_back(opponentUnit);
		opponentUnit->MakeUnitPushedState(true);
		opponentUnit->Damaged(ownerUnit->GetGameObject(), ownerUnit->DetermineAttackDamage(m_ap));
		opponentUnit->GetGameObject()->GetComponent<NavigationAgent>()->SetActive(false);
		Vector3d startPosition = opponentUnit->GetGameObject()->GetTransform()->GetWorldPosition();

		Vector3d directionVector = (startPosition - GetGameObject()->GetTransform()->GetWorldPosition()).Normalized();
		Vector3d endPosition = startPosition + (directionVector * m_pushPower);


		opponentUnit->knockBackTimer->m_duration = m_duration;
		Vector3d unitCurrentPos = opponentUnit->GetTransform()->GetWorldPosition();
		opponentUnit->knockBackTimer->onUpdate = [=](float normT)
		{
			Vector3d finalPos =
				Vector3d::Lerp(unitCurrentPos, endPosition, getEasingFunction(easing_functions::EaseOutQuad)(normT))
				+ Vector3d::up* maxKnockHeight * EasingBackFunction(easing_functions::EaseOutCubic, easing_functions::EaseOutCubic,normT);
			opponentUnit->GetTransform()->SetWorldPosition(finalPos);
		};
		opponentUnit->knockBackTimer->onCompleteFunction= [=]()
		{
			opponentUnit->MakeUnitPushedState(false);
			opponentUnit->DetermineCurrentTargetObject();
			opponentUnit->GetGameObject()->GetComponent<NavigationAgent>()->SetActive(true);
			opponentUnit->GetGameObject()->GetComponent<NavigationAgent>()->AssignToNavigationField(opponentUnit->GetNavField());
			opponentUnit->GetGameObject()->GetComponent<NavigationAgent>()->Relocate(endPosition);
		};
		opponentUnit->knockBackTimer->ActivateTimer();
		//opponentUnit->knockBackDotween->DOMove(endPosition, m_duration).SetEase(EaseOutQuart).OnComplete([=]()
		//	{

		//	});
	}
}

void KnockBackComponent::Update()
{

}

void KnockBackComponent::SetAP(float p_ap)
{
	m_ap = p_ap;
}

void KnockBackComponent::SkillStarted()
{
	isSkillStarted = true;
}

void KnockBackComponent::SkillEnded()
{
	isSkillStarted = false;
	crushedUnitList.clear();
}

