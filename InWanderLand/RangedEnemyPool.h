#pragma once
#include "YunutyEngine.h"
#include "DummyComponent.h"
#include "MeleeEnemyProductor.h"
#include "ContentsLayer.h"
#include "Application.h"
#include "LazySingletonClass.h"
#include "RangedEnemyProductor.h"

class RangedEnemyPool : public GameObjectPool<DummyComponent>, public GHContents::LazySingletonClass<RangedEnemyPool>
{
private:
	Vector3d m_unitPosition;
public:
	virtual void ObjectInitializer(DummyComponent* p_dummy) override
	{
		auto unitComponent = RangedEnemyProductor::Instance().CreateUnit(Vector3d::zero);
		unitComponent->m_dummyCom = p_dummy;
		unitComponent->returnToPoolFunction = [=]()
			{
				Return(p_dummy);
			};
		p_dummy->m_pairUnit = unitComponent;
		p_dummy->GetGameObject()->SetParent(unitComponent->GetGameObject());
		application::contents::ContentsLayer* contentsLayer = dynamic_cast<application::contents::ContentsLayer*>(application::Application::GetInstance().GetContentsLayer());
		contentsLayer->RegisterToEditorObjectVector(p_dummy->GetGameObject());
		//contentsLayer->RegisterToEditorObjectVector(unitComponent->GetGameObject());
	}

	virtual void OnBorrow(DummyComponent* p_dummy) override
	{
		NavigationAgent* tempNav = p_dummy->m_pairUnit->GetGameObject()->GetComponent<NavigationAgent>();
		tempNav->SetActive(false);
		p_dummy->m_pairUnit->GetTransform()->SetWorldPosition(m_unitPosition);
		tempNav->SetActive(true);
		tempNav->AssignToNavigationField(&SingleNavigationField::Instance());
		tempNav->Relocate(m_unitPosition);
	}

	void SetStartPosition(Vector3d p_pos)
	{
		m_unitPosition = p_pos;
	}
};