#pragma once
#include "YunutyEngine.h"
#include "DummyComponent.h"
#include "ContentsLayer.h"
#include "Application.h"
#include "LazySingletonClass.h"
#include "RangedEnemyProductor.h"

class RangedEnemyPool : public GameObjectPool<DummyComponent>, public GHContents::LazySingletonClass<RangedEnemyPool>
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
		unitComponent->returnToPoolFunction = [=]()
			{
				p_dummy->m_pairUnit->m_navAgentComponent->DetachFromNavigationField();
				Return(p_dummy);
				//p_dummy->m_pairUnit->m_navAgentComponent->DetachFromNavigationField();
			};
		//unitComponent->GetGameObject()->SetParent(p_dummy->GetGameObject());
		application::contents::ContentsLayer* contentsLayer = dynamic_cast<application::contents::ContentsLayer*>(application::Application::GetInstance().GetContentsLayer());
		contentsLayer->RegisterToEditorObjectVector(p_dummy->GetGameObject());
		contentsLayer->RegisterToEditorObjectVector(unitComponent->GetGameObject());
	}

	virtual void OnBorrow(DummyComponent* p_dummy) override
	{
		p_dummy->m_pairUnit->ResetUnitMembers();
		p_dummy->m_pairUnit->m_navAgentComponent;
		//tempNav->DetachFromNavigationField();
		//tempNav->SetActive(false);
		p_dummy->m_pairUnit->GetTransform()->SetWorldPosition(m_unitPosition);
		p_dummy->GetGameObject()->SetSelfActive(true);
		p_dummy->m_pairUnit->m_navAgentComponent->SetActive(true);
		p_dummy->m_pairUnit->m_navAgentComponent->AssignToNavigationField(p_dummy->m_pairUnit->GetNavField());
		p_dummy->m_pairUnit->m_navAgentComponent->Relocate(m_unitPosition);

	}

	void SetStartPosition(Vector3d p_pos)
	{
		m_unitPosition = p_pos;
	}

	void SetStageNumber(int p_num)
	{
		m_stageNumber = p_num;
	}
};