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
public:
	void SetStartPosition(Vector3d pos);
	virtual void ObjectInitializer(DummyComponent* p_dummy) override
	{
		auto unitComponent = RangedEnemyProductor::Instance().CreateUnit(m_startPos);
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

void RangedEnemyPool::SetStartPosition(Vector3d pos)
{
	m_startPos = pos;
}
