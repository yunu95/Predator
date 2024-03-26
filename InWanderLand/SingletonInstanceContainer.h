#pragma once
#include "YunutyEngine.h"
#include "LazySingletonClass.h"
/// <summary>
/// LazySingletonClass의 instance들을 담는 클래스.
/// 객체들을 일괄적으로 관리해준다.
/// </summary>

template<typename T>
class SingletonInstanceContainer : public SingletonClass<SingletonInstanceContainer<T>>
{
private:
	std::vector<T*> instanceVector;

public:
	void RegisterSingleton(T* p_instance);
	void ClearVector();
	void RegenerateInstances();
};

template<typename T>
void SingletonInstanceContainer<T>::RegenerateInstances()
{
	for (auto e : instanceVector)
	{
		e->PermitCreateInstance();
	}
	instanceVector.clear();
}

template<typename T>
void SingletonInstanceContainer<T>::RegisterSingleton(T* p_instance)
{
	instanceVector.push_back(p_instance);
}

template<typename T>
void SingletonInstanceContainer<T>::ClearVector()
{
	for (auto e : instanceVector)
	{
		e->ProhibitCreateInstance();
		e = nullptr;
	}
}


