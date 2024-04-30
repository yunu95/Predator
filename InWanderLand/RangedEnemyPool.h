#pragma once
#include "YunutyEngine.h"
#include "DummyComponent.h"
#include "RangedEnemyProductor.h"
#include "BurnEffect.h"
#include "ContentsObservee.h"

class RangedEnemyPool : public GameObjectPool<DummyComponent>, public Component, public SingletonComponent<RangedEnemyPool>, public ContentsObservee
{
private:
	Vector3d m_unitPosition;
	int m_stageNumber;

public:
	virtual void ObjectInitializer(DummyComponent* p_dummy) override
	{
		auto unitComponent = RangedEnemyProductor::Instance().CreateUnit(Vector3d::zero);
		unitComponent->m_dummyCom = p_dummy;
		unitComponent->stageNumber = m_stageNumber;
		p_dummy->m_pairUnit = unitComponent;
		p_dummy->m_pairUnit->isJustCreated = true;
		unitComponent->returnToPoolFunction = [=]()
			{
				p_dummy->m_pairUnit->m_navAgentComponent->SetActive(false);
				p_dummy->m_pairUnit->isJustCreated = false;
				Return(p_dummy);
			};
	}

	virtual void OnBorrow(DummyComponent* p_dummy) override
	{
		p_dummy->m_pairUnit->ResetUnitMembers();
		p_dummy->m_pairUnit->GetTransform()->SetWorldPosition(m_unitPosition);
		p_dummy->m_pairUnit->GetGameObject()->GetComponent<BurnEffect>()->Appear();
		p_dummy->m_pairUnit->GetGameObject()->SetSelfActive(true);
		p_dummy->m_pairUnit->m_navAgentComponent->SetActive(true);
		p_dummy->m_pairUnit->m_navAgentComponent->AssignToNavigationField(p_dummy->m_pairUnit->GetNavField());
		p_dummy->m_pairUnit->m_navAgentComponent->Relocate(m_unitPosition);
		p_dummy->m_pairUnit->m_navAgentComponent->MoveTo(m_unitPosition);
		p_dummy->m_pairUnit->SetUnitStateIdle();
		p_dummy->m_pairUnit->StopMove();

		if (!p_dummy->m_pairUnit->isJustCreated)
		{
			for (auto each : p_dummy->m_pairUnit->OnCreated)
			{
				each();
			}
		}
	}

	void SetStartPosition(Vector3d p_pos)
	{
		m_unitPosition = p_pos;
	}

	void SetStageNumber(int p_num)
	{
		m_stageNumber = p_num;
	}

	virtual void Start() override;
	virtual void PlayFunction() override;
	virtual void StopFunction() override;
};

void RangedEnemyPool::Start()
{
	isSingletonComponent = true;
}

void RangedEnemyPool::PlayFunction()
{
	this->SetActive(true);
	if (isOncePaused)
	{
		Start();
	}
}

void RangedEnemyPool::StopFunction()
{
	poolObjects.clear();
	expendableObjects.clear();
}
