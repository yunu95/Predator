#pragma once

/// <summary>
/// 객체 생성 조건에 bool값을 하나 추가해 특정함수를 호출했을 경우엔
/// SingleInstance()를 호출해도 객체를 생성하지 않는 싱글톤.
/// </summary>
namespace GHContents
{
	template<typename T>
	class LazySingletonClass
	{
	public:
		static T& SingleInstance();
		static void ProhibitCreateInstance();
		static void PermitCreateInstance();
		static void InitInstance();
		LazySingletonClass(const LazySingletonClass&) = delete;
		LazySingletonClass& operator=(const LazySingletonClass&) = delete;
	protected:
		LazySingletonClass() = default;
		virtual ~LazySingletonClass() = default;
	private:
		static T* instance;
		static inline bool isPermitted = true;
	};
}
template<typename T>
T& GHContents::LazySingletonClass<T>::SingleInstance()
{
	if (instance == nullptr && isPermitted)
	{
		instance = new T();
	}
	return *instance;
}

template<typename T>
void GHContents::LazySingletonClass<T>::ProhibitCreateInstance()
{
	isPermitted = false;
}

template<typename T>
void GHContents::LazySingletonClass<T>::PermitCreateInstance()
{
	isPermitted = true;
}

template<typename T>
void GHContents::LazySingletonClass<T>::InitInstance()
{
	instance = nullptr;
}

template<typename T>
T* GHContents::LazySingletonClass<T>::instance = nullptr;