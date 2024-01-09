#pragma once
#include "YunutyEngine.h"
/// <summary>
/// 키와 마우스의 입력을 받으면 해당 입력에 따라 다른 클래스의 함수를 호출해주는 함수.
///	입력과 전달의 역할만 갖는다.
/// </summary>
class InputManager : public Component
{
public:
	enum SelectedSerialNumber
	{
		One = 1,
		Two,
		Three,
		All
	};

private:
	int currentSelectedSerialNumber;

public:
	virtual void Start() override;
	virtual void Update() override;

};

