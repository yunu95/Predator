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

	StatusTimer* bleedingTimer = StatusTimerPool::SingleInstance().Borrow();
	auto debuggingMesh = DebuggingMeshPool::SingleInstance().Borrow();
	debuggingMesh->SetUnitObject(opponentUnit);

	bool isAlreadyBleeding = false;

	// 1. 우선 stl에 들어가 있는지 부터 확인.
	for (auto e : opponentUnitMap)
	{
		if (e.first->bleedingUnit == opponentUnit)
		{
			isAlreadyBleeding = true;
			if (e.first->currentBleedingStack < maxBleedingStack)
			{
				e.first->currentBleedingStack++;			// 스택 증가 (중첩 증가)
				e.first->currentDamagedCount = 0;			// 상태 초기화
			}
		}
	}
	
	// 2. 들어가 있지 않다면 넣어주기
	if (!isAlreadyBleeding)
	{
		bleedingUnitInfo = new bleededUnitInfo;
		bleedingUnitInfo->bleedingUnit = opponentUnit;
		bleedingUnitInfo->currentBleedingStack = 1;

		bleedingTimer->m_isRepeated = true;
		bleedingTimer->m_duration = m_bleedDuration;
		bleedingTimer->onCompleteFunction = [=]()
		{
			if (bleedingUnitInfo->currentDamagedCount == m_maxDamageCount)
			{
				bleedingTimer->StopTimer();
				opponentUnitMap.erase(bleedingUnitInfo);
				delete bleedingUnitInfo;
			}

			else
			{
				bleedingUnitInfo->bleedingUnit->Damaged(m_bleedDamage);
				debuggingMesh->PopMeshUP(yunuGI::Color::red(), MaterialNum::Red);
				bleedingUnitInfo->currentDamagedCount++;
			}
		};

		bleedingTimer->ActivateTimer();
		opponentUnitMap.insert({ bleedingUnitInfo, bleedingTimer });
	}	
}

void BleedingComponent::Update()
{
	
}

