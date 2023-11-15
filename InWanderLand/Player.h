#pragma once
#include "Unit.h"
/// <summary>
/// Player 클래스. 추후 Unit을 상속받는 구조로 수정 예정.
/// </summary>

class RTSCam;

class Player : public Unit
{
private:
	RTSCam* rtsCamComponent;

	bool changeToMoveState;

	Vector3d m_movePosition;

public:
	virtual void Start() override;
	virtual void Update() override;

	void SetMovingSystemComponent(RTSCam* sys);

private:
	virtual void MoveEngage() override;

	void MoveStateFunction();


};

 