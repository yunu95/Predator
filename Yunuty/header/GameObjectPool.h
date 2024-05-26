#pragma once
#include "Component.h"
#include "GameObject.h"
#include "Scene.h"
#include "SingletonClass.h"
#include <functional>
#include <unordered_set>
#include <vector>

#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif

using namespace std;
using namespace yunutyEngine;
namespace yunutyEngine
{
	// 오브젝트가 필요하면 생성하고, 생성된 오브젝트의 용도가 다 끝나면 폐기하는 대신 비활성화만 시켜놨다가,
	// 다시 오브젝트에 대한 요청이 들어오면 재활성화 시키는 오브젝트 풀 객체입니다. 
	template<typename RepresenstativeComponent>
	class GameObjectPool
	{
		static_assert(std::is_base_of<Component, RepresenstativeComponent>::value, "only derived classes from component are allowed");
	public:
		// 빌려줄 오브젝트가 단 하나도 없을 경우, 활성화된 씬에서 게임 오브젝트를 생성한 뒤 RepresentativeComponent를 붙여줍니다.
		// 이 작업 이후 추가적으로 실행할 초기화 함수를 정의합니다.
		virtual void ObjectInitializer(std::weak_ptr<RepresenstativeComponent> comp) = 0;
		// 오브젝트를 하나 빌렸을 때 실행될 함수를 정의합니다. 굳이 정의 안해도 됩니다.
		virtual void OnBorrow(std::weak_ptr<RepresenstativeComponent> comp) {};
		// 게임 오브젝트 풀에 저장된 게임 오브젝트를 활성화합니다.
		std::weak_ptr<RepresenstativeComponent> Borrow();
		// 게임 오브젝트 풀에서 관리하는 게임 오브젝트를 되돌려 줍니다.
		void Return(std::weak_ptr<RepresenstativeComponent>);
		int poolObjectsSize() { return poolObjects.size(); };
		int expendableObjectsSize() { return expendableObjects.size(); };

	protected:
		const unordered_set<std::weak_ptr<RepresenstativeComponent>>& GetPoolObjects() {
			return poolObjects;
		}
	protected:
		unordered_set<std::weak_ptr<RepresenstativeComponent>> poolObjects;
		unordered_set<std::weak_ptr<RepresenstativeComponent>> expendableObjects;
	};
}
template<typename RepresenstativeComponent>
std::weak_ptr<RepresenstativeComponent> GameObjectPool<RepresenstativeComponent>::Borrow()
{
	if (expendableObjects.empty())
	{
		auto gameObject = Scene::getCurrentScene()->AddGameObject();
		auto component = gameObject->AddComponentAsWeakPtr<RepresenstativeComponent>();
		ObjectInitializer(component);
		poolObjects.insert(component);
		expendableObjects.insert(component);
	}
	auto target = *expendableObjects.begin();
	expendableObjects.erase(target);
	OnBorrow(target);
	target->GetGameObject()->SetSelfActive(true);
	return target;
}
template<typename RepresenstativeComponent>
void GameObjectPool<RepresenstativeComponent>::Return(std::weak_ptr<RepresenstativeComponent> obj)
{
	if (poolObjects.find(obj) != poolObjects.end())
	{
		expendableObjects.insert(obj);
		obj->lock()->GetGameObject()->SetSelfActive(false);
	}
}
