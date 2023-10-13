#pragma once
#include "IRangeAction.h"

/// <summary>
/// 일반 적군 유닛 클래스이자 플레이어 및 보스 클래스의 부모 클래스.
/// 기본적으로 모든 유닛이 갖는 기능을 갖고 있다.
/// </summary>
class Unit : public IRangeAction
{
public:
	// 컴포넌트에서 가져온 함수
	virtual void Start() override;
	virtual void Update() override;

public:
	// IRangeAction에서 가져온 함수
	virtual void IdentifyAction(Vector3d position) override;
	virtual void AttackAction(Vector3d position) override;

public:
	// 멤버함수들
};

