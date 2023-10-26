#pragma once
#include "Unit.h"
/// <summary>
/// Player 클래스. 추후 Unit을 상속받는 구조로 수정 예정.
/// </summary>
class Player : public Unit
{
public:
	Player();

public:
	virtual void Start() override;
	virtual void Update() override;

public:

public:
};

