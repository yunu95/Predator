#pragma once
#include "YunutyEngine.h"

/// <summary>
/// HealerAutoAttackProjectile에 멤버로 등록될 컴포넌트.
/// 공격횟수가 n번 일 경우 PassiveCake의 함수 호출.
/// </summary>

class Dotween;

class PassiveCake : public ContentsObservee
{
private:
	yunutyEngine::graphics::StaticMeshRenderer* m_mesh;
	physics::BoxCollider* m_collider;
	Dotween* m_dotween;

	int maxDropDistance = 10;
	int minDropDistance = 2;
	int maxDegree = 359;
	float cakeHealingPoint = 1.0f;

	float cakePersistTime = 3.5f;
	float cakeElapsed = 0.0f;

	bool isFunctionCalled = false;
	bool isDropped = false;

public:
	void PopCake(Vector3d pos);

	void SetMesh(yunutyEngine::graphics::StaticMeshRenderer* p_mesh);
	void SetCollider(physics::BoxCollider* p_collider);
	void SetDotweenComponent(Dotween* p_dotween);

	virtual void Start() override;
	virtual void Update() override;
	virtual void OnTriggerEnter(physics::Collider* collider) override;
};

