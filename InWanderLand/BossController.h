/// 2024. 06. 09 김상준
/// Boss 패턴 구현을 위한 Controller

#pragma once

#include "EnemyController.h"

class BossController
	: public EnemyController, public SingletonComponent<BossController>
{
public:
    virtual void RegisterUnit(std::weak_ptr<Unit> unit) override;
	virtual void OnContentsStop() override;

	std::weak_ptr<Unit> GetBoss();

private:
	int currentState = 0;
	int beforeSkillIndex = 0;
	virtual coroutine::Coroutine RoutinePerUnit(std::weak_ptr<Unit> unit)override;
};