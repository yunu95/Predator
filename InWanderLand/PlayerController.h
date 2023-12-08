#pragma once
#include "YunutyEngine.h"
/// <summary>
/// Unit Component가 부착된 Gameobject에다가 붙여주면 조작이 가능해지도록 해주는 컴포넌트.
/// </summary>

class RTSCam;

class PlayerController : public Component
{
private:
	RTSCam* m_movingSystemComponent;

public:
	void SetMovingSystemComponent(RTSCam* sys);

	virtual void Start() override;
};

