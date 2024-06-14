#pragma once

// 이 인터페이스는 전술모드에서 정지했다가 다시 재생이 되어야하는 class가 상속받아
// 아래의 두 순수 가상 함수를 재정의합니다.
class ITacticObject
{
public:
	virtual void OnPause() = 0;
	virtual void OnResume() = 0;
};