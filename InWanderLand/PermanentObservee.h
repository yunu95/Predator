#pragma once
#include "ContentsObservee.h"
/// <summary>
/// 해당 클래스를 상속받는 컴포넌트는
/// 에디터 실행 정지 및 인게임 Restart 시에도 사라지지 않습니다.
/// </summary>
class PermanentObservee : public ContentsObservee
{
public:
	PermanentObservee();
};

