#pragma once
#include "YunutyEngine.h"
#include "LazySingletonClass.h"
/// <summary>
/// LazySingletonClass의 instance들을 담는 클래스.
/// 객체들을 일괄적으로 관리해준다.
/// </summary>

class SingletonInstanceContainer : public SingletonClass<SingletonInstanceContainer>
{
public:
	void ClearSingletonInstances();
	void PermitCreateInstances();
};

