#include "KnockBackComponent.h"
#include "UnitTransformComponent.h"
#include "Unit.h"
#include "Dotween.h"

void KnockBackComponent::ApplyStatus(Unit* ownerUnit, Unit* opponentUnit)
{
	/// 넉백 시스템
	/// opponent 유닛을 밀어 내야 하는데...
	/// navigationAgent를 끄고 DOMove 후 OnComplete 에 navigationAgent를 붙여준다?

	opponentUnit->GetGameObject()->GetComponent<NavigationAgent>()->SetActive(false);
	Vector3d startPosition = opponentUnit->GetGameObject()->GetTransform()->GetWorldPosition();

	Vector3d directionVector = (startPosition - GetGameObject()->GetTransform()->GetWorldPosition()).Normalized();
	Vector3d endPosition = startPosition + (directionVector * m_power);

	opponentUnit->GetGameObject()->GetComponent<Dotween>()->DOMove(endPosition, m_duration).SetEase(EaseOutQuart).OnComplete([=]()
		{
			opponentUnit->GetGameObject()->GetComponent<NavigationAgent>()->SetActive(true);
			opponentUnit->GetGameObject()->GetComponent<NavigationAgent>()->AssignToNavigationField(opponentUnit->GetNavField());
			opponentUnit->GetGameObject()->GetComponent<NavigationAgent>()->Relocate(endPosition);
		});
}

void KnockBackComponent::Update()
{

}

