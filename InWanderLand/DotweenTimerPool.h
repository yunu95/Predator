#pragma once
#include <stack>
#include "YunutyEngine.h"
#include "PermanentObservee.h"
#include "DotweenTimer.h"

/// <summary>
/// DO함수를 쓸 때 마다 new 해주고,
/// 다 쓰고 나서 delete 해주는 것은 효율적이지 않다.
/// 미리 DotweenTimer를 50개 정도 생성해주고 가져다 써보자.
/// </summary>

class DotweenTimer;

class DotweenTimerPool : public GameObjectPool<DotweenTimer>, public SingletonComponent<DotweenTimerPool>, public Component, public PermanentObservee
{
private:

public:
	virtual void ObjectInitializer(DotweenTimer* timer) override
	{
		timer->Init();
	}

	virtual Component* GetComponent() override { return this; }

	virtual void OnContentsStop() override
	{
		this->SetActive(false);
		poolObjects.clear();
		expendableObjects.clear();
	};
};

