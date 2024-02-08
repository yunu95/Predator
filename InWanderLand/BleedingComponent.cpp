#include "InWanderLand.h"
#include "BleedingComponent.h"
#include "StatusTimerPool.h"
#include "StatusTimer.h"
#include "Unit.h"
#include "DebuggingMeshPool.h"

void BleedingComponent::ApplyStatus(Unit* ownerUnit, Unit* opponentUnit)
{
	/// 출혈 시스템	
	/// damage를 주는건 Unit->Damaged 함수를 호출,
	/// 이외의 로직(출혈 스택, 지속 피해, )은 내부에서 구현	
	/// 기존에 출혈을 입고 있는 유닛이면 count 초기화 및 중첩 증가

	// 1. 우선 map에 들어가 있는지 부터 확인.
	// 상수시간 탐색
	if (auto found = opponentUnits.find(opponentUnit); found != opponentUnits.end())
	{
		
		found->second->currentDamagedCount = 0;			// 상태 초기화

		if (found->second->currentBleedingStack < maxBleedingStack)
		{
			found->second->currentBleedingStack++;			// 스택 증가 (중첩 증가)
		}

		found->second->statusTimer->m_elapsed = 0.0f;
		found->second->statusTimer->ActivateTimer();
	}
	// 2. 들어가 있지 않다면 넣어주기
	else
	{
		//std::unique_ptr<bleededUnitInfo> bleedingUnitInfo = make_unique<bleededUnitInfo>();

		opponentUnits.insert(std::make_pair(opponentUnit, make_unique<bleededUnitInfo>()));

		opponentUnits.find(opponentUnit)->second->bleedingUnit = opponentUnit;
		opponentUnits.find(opponentUnit)->second->currentBleedingStack = 1;
		opponentUnits.find(opponentUnit)->second->currentDamagedCount = 0;

		StatusTimer* bleedingTimer = StatusTimerPool::SingleInstance().Borrow();
		bleedingTimer->m_isRepeated = true;
		bleedingTimer->m_duration = m_bleedDuration;
		bleedingTimer->onCompleteFunction = [=]()
		{
			/// 출혈의 지속시간이 종료되거나 유닛이 사망하면 erase해준다.
			if (opponentUnits.find(opponentUnit)->second->currentDamagedCount == m_maxDamageCount || opponentUnits.find(opponentUnit)->second->bleedingUnit->IsUnitDead())
			{
				StatusTimerPool::SingleInstance().Return(bleedingTimer);
				opponentUnits.erase(opponentUnits.find(opponentUnit)->second->bleedingUnit);
			}
			else
			{
				opponentUnits.find(opponentUnit)->second->bleedingUnit->Damaged(m_bleedDamage * opponentUnits.find(opponentUnit)->second->currentBleedingStack);
				opponentUnits.find(opponentUnit)->second->currentDamagedCount++;

				auto debuggingMesh = DebuggingMeshPool::SingleInstance().Borrow();
				debuggingMesh->SetUnitObject(opponentUnits.find(opponentUnit)->second->bleedingUnit);
				debuggingMesh->GetGameObject()->SetSelfActive(true);
				debuggingMesh->PopMeshUP(yunuGI::Color::red(), MaterialNum::Red);
			}
		};

		auto temp = &bleedingTimer->onCompleteFunction;

		bleedingTimer->ActivateTimer();
		opponentUnits.find(opponentUnit)->second->statusTimer = bleedingTimer;

	}
}

void BleedingComponent::Update()
{

}

