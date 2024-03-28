#pragma once
#include "YunutyEngine.h"
#include "DummyComponent.h"
#include "MeleeEnemyProductor.h"
#include "ContentsLayer.h"
#include "Application.h"
#include "LazySingletonClass.h"

class MeleeEnemyPool : public GameObjectPool<DummyComponent>, public GHContents::LazySingletonClass<MeleeEnemyPool>
{
private:
	Vector3d m_startPos;
public:
	void SetStartPosition(Vector3d pos);
	virtual void ObjectInitializer(DummyComponent* p_dummy) override
	{
		auto unitComponent = MeleeEnemyProductor::Instance().CreateUnit(m_startPos);
		unitComponent->returnToPoolFunction = [=]()
			{
				Return(p_dummy);
			};
		p_dummy->m_pairUnit = unitComponent;
		//p_dummy->GetGameObject()->SetParent(unitComponent->GetGameObject());
		application::contents::ContentsLayer* contentsLayer = dynamic_cast<application::contents::ContentsLayer*>(application::Application::GetInstance().GetContentsLayer());
		contentsLayer->RegisterToEditorObjectVector(unitComponent->GetGameObject());
		contentsLayer->RegisterToEditorObjectVector(unitComponent->GetGameObject());
	}
};

void MeleeEnemyPool::SetStartPosition(Vector3d pos)
{
	m_startPos = pos;
}
