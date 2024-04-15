#pragma once
#include <stack>
#include "GameObjectPool.h"
#include "StatusTimer.h"
#include "ContentsLayer.h"
#include "Application.h"
#include "LazySingletonClass.h"

class Unit;
class StatusTimer;

class StatusTimerPool : public GameObjectPool<StatusTimer>, public GHContents::LazySingletonClass<StatusTimerPool>
{
public:
	virtual void ObjectInitializer(StatusTimer* timer) override
	{
		application::contents::ContentsLayer* contentsLayer = dynamic_cast<application::contents::ContentsLayer*>(application::Application::GetInstance().GetContentsLayer());
		contentsLayer->RegisterToEditorObjectContainer(timer->GetGameObject());
	}
};

