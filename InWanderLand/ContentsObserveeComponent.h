/// 2024. 05. 17 김상준
/// ContentsObservee 를 상속받은 기본 형태의 Component 입니다.

#pragma once

#include "YunutyEngine.h"
#include "ContentsObservee.h"

class ContentsObserveeComponent
	: public Component, public ContentsObservee
{
	virtual Component* GetComponent() { return this; }
};
