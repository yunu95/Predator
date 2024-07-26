/// 2024. 06. 09 김상준
/// Boss 패턴 구현을 위한 Controller

#pragma once

#include "EnemyController.h"
#include "ProgressTracker.h"

namespace application
{
	namespace editor
	{
		class UnitData;
	}
}

class BossController
	: public EnemyController, public SingletonComponent<BossController>, public application::ProgressTracker
{
public:
    virtual void RegisterUnit(std::weak_ptr<Unit> unit) override;
	virtual void OnContentsStop() override;

	void BossAppear();

	std::weak_ptr<Unit> GetBoss();

	std::vector<std::function<void()>> onBossDieCallback = std::vector<std::function<void()>>();

	virtual void ProgressInitialize();
	virtual void CurrentProgressSave();
	virtual void Recovery();
	virtual void PostRecovery();

private:
	void ChangeAttackTarget(const std::weak_ptr<Unit>& unit);
	coroutine::Coroutine BossAppearCoroutine();
	coroutine::Coroutine BossAppearEffectCoroutine();

	std::weak_ptr<coroutine::Coroutine> appearEffectCorountine = std::weak_ptr<coroutine::Coroutine>();
	std::weak_ptr<Unit> boss = std::weak_ptr<Unit>();
	application::editor::UnitData* bossData = nullptr;
	std::weak_ptr<ManagedFBX> appearEffect = std::weak_ptr<ManagedFBX>();

	int summonState = 0;
	bool summonDone = true;
	int currentState = 0;
	int beforeSkillIndex = 0;
	virtual coroutine::Coroutine RoutinePerUnit(std::weak_ptr<Unit> unit) override;
	bool unitRoutineFlag = false;
};