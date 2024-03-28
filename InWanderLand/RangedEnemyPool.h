#pragma once
#include "YunutyEngine.h"
#include "DummyComponent.h"
#include "RangedEnemyProductor.h"
#include "ContentsLayer.h"
#include "Application.h"
#include "LazySingletonClass.h"

class RangedEnemyPool : public GameObjectPool<DummyComponent>, public GHContents::LazySingletonClass<RangedEnemyPool>
{
private:
	Vector3d m_startPos;
	Unit* m_unitComponent{ nullptr };
public:
	void SetStartPosition(Vector3d pos);
	Unit* GetUnitComponent() const;
	virtual void ObjectInitializer(DummyComponent* p_dummy) override
	{
		m_unitComponent = RangedEnemyProductor::Instance().CreateUnit(m_startPos);
		m_unitComponent->returnToPoolFunction = [=]()
			{
				Return(p_dummy);
			};
		m_unitComponent->GetGameObject()->SetParent(p_dummy->GetGameObject());
		application::contents::ContentsLayer* contentsLayer = dynamic_cast<application::contents::ContentsLayer*>(application::Application::GetInstance().GetContentsLayer());
		contentsLayer->RegisterToEditorObjectVector(p_dummy->GetGameObject());
		//contentsLayer->RegisterToEditorObjectVector(m_unitComponent->GetGameObject());
	}
};

void RangedEnemyPool::SetStartPosition(Vector3d pos)
{
	m_startPos = pos;
}

Unit* RangedEnemyPool::GetUnitComponent() const
{
	return m_unitComponent;
}
