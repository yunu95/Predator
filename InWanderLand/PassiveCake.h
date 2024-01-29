#pragma once
#include "YunutyEngine.h"

/// <summary>
/// HealerAutoAttackProjectile에 멤버로 등록될 컴포넌트.
/// 공격횟수가 n번 일 경우 PassiveCake의 함수 호출.
/// </summary>
class PassiveCake : public Component
{
private:
	yunutyEngine::graphics::StaticMeshRenderer* m_mesh;
	physics::BoxCollider* m_collider;

	int maxDropDistance = 10;
	int minDropDistance = 2;
	int maxDegree = 359;
	float cakeHealingPoint = 1.0f;

	float cakePersistTime = 2.0f;
	float cakeElapsed = 0.0f;

	bool isDropped = false;

public:
	void PopCake(Vector3d pos);

	void SetMesh(yunutyEngine::graphics::StaticMeshRenderer* p_mesh);
	void SetCollider(physics::BoxCollider* p_collider);

	virtual void Start() override;
	virtual void Update() override;
	virtual void OnTriggerEnter(physics::Collider* collider) override;
};

