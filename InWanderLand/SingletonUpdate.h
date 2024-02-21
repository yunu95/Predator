#pragma once
#include "YunutyEngine.h"
#include "InputManager.h"
#include "SkillPreviewSystem.h"
#include "UIManager.h"

/// <summary>
/// Singleton 클래스들의 Update문 돌려주기 위한 임시 클래스.
/// </summary>
class SingletonUpdate : public Component
{
public:
	virtual void Update() override
	{
		SkillPreviewSystem::SingleInstance().Update();
		InputManager::SingleInstance().Update();
		UIManager::SingleInstance().Update();
	}
};