#pragma once
#include "YunutyEngine.h"
#include "DummyComponent.h"
#include "MeleeEnemyProductor.h"
#include "ContentsLayer.h"
#include "Application.h"
#include "LazySingletonClass.h"

class UnitObjectPool : public GameObjectPool<DummyComponent>, public GHContents::LazySingletonClass<UnitObjectPool>
{
private:
	Vector3d m_startPos;
	UnitProductor* m_selectedProductor;

public:
	void SetStartPosition(Vector3d pos);
	void ChooseProductor(UnitProductor* p_prod);
	virtual void ObjectInitializer(DummyComponent* p_dummy) override
	{
		auto unitComponent = m_selectedProductor->CreateUnit(m_startPos);
		unitComponent->returnToPoolFunction = [=]()
			{
				Return(p_dummy);
			};
		p_dummy->m_pairUnit = unitComponent;
		//p_dummy->GetGameObject()->SetParent(unitComponent->GetGameObject());
		application::contents::ContentsLayer* contentsLayer = dynamic_cast<application::contents::ContentsLayer*>(application::Application::GetInstance().GetContentsLayer());
		contentsLayer->RegisterToEditorObjectVector(p_dummy->GetGameObject());
		contentsLayer->RegisterToEditorObjectVector(unitComponent->GetGameObject());
	}
};

void UnitObjectPool::SetStartPosition(Vector3d pos)
{
	m_startPos = pos;
}

void UnitObjectPool::ChooseProductor(UnitProductor* p_prod)
{
	m_selectedProductor = p_prod;
}