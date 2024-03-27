#pragma once
#include "YunutyEngine.h"
#include "DummyComponent.h"
#include "MeleeEnemyProductor.h"
#include "ContentsLayer.h"
#include "Application.h"

class MeleeEnemyPool : public GameObjectPool<DummyComponent>, public SingletonClass<MeleeEnemyPool>
{
private:
	Vector3d m_startPos;
public:
	void SetStartPosition(Vector3d pos);

	virtual void ObjectInitializer(DummyComponent* p_dummy) override
	{
		Unit* unitCom = MeleeEnemyProductor::Instance().CreateUnit(m_startPos);
		unitCom->returnToPoolFunction = [=]()
			{
				Return(p_dummy);
			};
		unitCom->GetGameObject()->SetParent(p_dummy->GetGameObject());
		application::contents::ContentsLayer* contentsLayer = dynamic_cast<application::contents::ContentsLayer*>(application::Application::GetInstance().GetContentsLayer());
		contentsLayer->RegisterToEditorObjectVector(p_dummy->GetGameObject());
	}
};

void MeleeEnemyPool::SetStartPosition(Vector3d pos)
{
	m_startPos = pos;
}
