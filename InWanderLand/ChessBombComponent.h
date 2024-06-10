/// 2024. 05. 30 김상준
/// Chess 오브젝트에서 Damage 범위를 표시하고 해당 범위의 Unit 을 알려주며,
/// Particle 효과까지 모두 일괄적으로 제공하는 Component 입니다.

#pragma once

#include "YunutyEngine.h"

class Unit;

class ChessBombComponent
	: public Component
{
public:
	virtual void Start() override;
	virtual void Update() override;

	virtual void OnTriggerEnter(physics::Collider* collider) override;
	virtual void OnTriggerExit(physics::Collider* collider) override;

	void SetBombTime(float time) { bombTime = time; }

	yunutyEngine::coroutine::Coroutine AwakeBomb();

	std::unordered_set<Unit*>& GetUnitsInTrigger() { return unitSet; }

	/// Pool 에서 사용하기 위한 초기화 함수입니다.
	void OnReturn();

private:
	GameObject* guideObj = nullptr;
	GameObject* particleObj = nullptr;
	std::unordered_set<Unit*> unitSet = std::unordered_set<Unit*>();
	float bombTime = 0;
	bool coroutineStart = false;
	std::weak_ptr<yunutyEngine::coroutine::Coroutine> lastCoroutine = std::weak_ptr<yunutyEngine::coroutine::Coroutine>();
};

