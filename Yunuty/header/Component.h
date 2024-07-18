#pragma once
#include <iostream>
#include <string>
#include <objbase.h>
#include <coroutine>
#include "Object.h"
#include <guiddef.h>
#include <unordered_map>
#include <unordered_set>
#include <rpc.h>
#include "YunutyGenericHash.h"
#include "YunutyGenericEqual.h"
#include "YunutyStringConversion.h"
#include "yunutyCoroutine.h"
#include "YunutyYieldInstruction.h"
//#include "GameObject.h"

#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif

namespace yunutyEngine
{
    class GameObject;
    class YunutyCycle;
    class Transform;
    class Collision2D;
    class Collider2D;
    
    
    namespace physics
    {
        class Collision;
        class Collider;
    }



    /// <summary>
    /// Component는 게임 오브젝트들에게 배치될 수 있는 객체입니다.
    /// 게임 오브젝트는 해당 게임 오브젝트에 어떤 Component들이 배치되어 있느냐에 따라 그 성질이 정의된다고 볼 수 있습니다.
    /// </summary>
    class YUNUTY_API Component :
        public Object
    {
    public:
        template<typename ComponentType>
        static ComponentType* LoadReferenceByGUID(const char* guid)
        {
            UUID uuid;
            //CLSIDFromString(guid, &uuid);
            UuidFromStringA((RPC_CSTR)(guid), &uuid);
            return LoadReferenceByGUID<ComponentType>(uuid);
        }

        template<typename ComponentType>
        static ComponentType* LoadReferenceByGUID(const WCHAR* guid)
        {
            UUID uuid;
            CLSIDFromString(guid, &uuid);
            return LoadReferenceByGUID<ComponentType>(uuid);
        }


        template<typename ComponentType>
        static ComponentType* LoadReferenceByGUID(GUID guid)
        {
            if (auto found = guidPtrMap.find(guid); found != guidPtrMap.end())
                return dynamic_cast<ComponentType*>(found->second);
            else
                return nullptr;
        }

        GUID GetGUID();
        std::wstring GetGUIDWStr();

        // 비활성화된 게임 오브젝트가 활성화 될 때 호출되는 콜백 함수입니다.
        virtual void OnEnable() {};
        // 활성화된 게임 오브젝트가 비활성화 될 때 호출되는 콜백 함수입니다.
        virtual void OnDisable() {};
        // 게임 실행 중 매 프레임마다 호출되는 콜백 함수입니다. Update 함수를 오버라이드 하고 싶으면 base::Update 함수는 부르지 마십시오.
        virtual void Update() { SetIsUpdating(false); };
        // 게임 실행 중 트랜스폼 값이 업데이트될때마다 호출되는 콜백 함수입니다. 부모의 트랜스폼이 바뀌어도 호출됩니다.
        virtual void OnTransformUpdate() {};
        // 컴포넌트로부터 최초로 Update가 호출될 때, 호출되는 콜백 함수입니다.
        virtual void Start() {};
        // 게임 오브젝트가 삭제될 때 호출되는 콜백 함수입니다.
        virtual void OnDestroy() {};
        // 게임 실행 중 정확한 시간 간격으로 호출되는 콜백 함수입니다. 아직 구현되지 않았습니다.
        virtual void FixedUpdate() {};
        // 충돌체 간에 이벤트가 일어날 때, 콜라이더가 배치된 게임 오브젝트가 갖고 있는 모든 Component들에게
        // 해당 이벤트에 대응되는 콜백 함수가 호출됩니다.

        // 리지드바디들끼리 충돌을 시작할 때 호출되는 콜백 함수입니다.
        virtual void OnCollisionEnter(const physics::Collision& collision) {};
        // 2D 콜라이더가 다른 2D 콜라이더와 겹쳐지기 시작할 때 호출되는 콜백 함수입니다.
        virtual void OnCollisionEnter2D(const Collision2D& collision) {};
        // 리지드바디들끼리 겹쳐진 상태로 있을때 매 프레임마다 호출되는 콜백 함수입니다.
        virtual void OnCollisionStay(const physics::Collision& collision) {};
        // 2D 콜라이더가 다른 2D 콜라이더와 서로 겹쳐진 상태로 있을때 매 프레임마다 호출되는 콜백 함수입니다.
        virtual void OnCollisionStay2D(const Collision2D& collision) {};
        // 리지드바디들끼리 충돌을 마치고 서로 떨어져 나갈때 호출되는 콜백 함수입니다.
        virtual void OnCollisionExit(const physics::Collision& collision) {};
        // 2D 콜라이더가 다른 2D 콜라이더로부터 떨어져 나갈때 호출되는 콜백 함수입니다.
        virtual void OnCollisionExit2D(const Collision2D& collision) {};

        // 콜라이더가 트리거 콜라이더와 충돌을 시작할 때 호출되는 콜백 함수입니다.
        virtual void OnTriggerEnter(physics::Collider* collider) {};
        // 콜라이더가 트리거 콜라이더와 서로 겹쳐진 상태로 있을때 매 프레임마다 호출되는 콜백 함수입니다.
        //virtual void OnTriggerStay(const physics::Collider* collider) {};

        // 콜라이더가 트리거 콜라이더로부터 떨어져 나갈때 호출되는 콜백 함수입니다. 상대 콜라이더가 삭제되면서 호출될 경우, collider는 nullptr이 됩니다.
        virtual void OnTriggerExit(physics::Collider* collider) {};
        virtual ~Component();
        Transform* GetTransform();
        GameObject* GetGameObject();
        const Transform* GetTransform()const;
        const GameObject* GetGameObject()const;
        // 컴포넌트의 활성화 여부를 가져옵니다.
        bool GetActive()const;
        // 컴포넌트를 비활성화합니다. OnEnable, OnDisable 콜백 함수가 호출됩니다.
        void SetActive(bool active);
        // 컴포넌트의 WeakPtr를 가져옵니다.
        template<typename T>
        std::weak_ptr<T> GetWeakPtr() const { return std::dynamic_pointer_cast<T>(GetWeakPtrFromGameObject().lock()); };
        // Update함수를 부를지 말지 결정합니다. isUpdate가 false면 게임 사이클에서 업데이트 함수가 불릴때 제외됩니다.
        void SetIsUpdating(bool isUpdating);
        std::weak_ptr<yunutyEngine::coroutine::Coroutine> StartCoroutine(coroutine::Coroutine&& coroutine);
        void DeleteCoroutine(const std::weak_ptr<coroutine::Coroutine>& coroutine);
        void ClearCoroutines();

        Component(const Component&) = delete;
        Component& operator=(const Component&) = delete;
        friend GameObject;
        friend YunutyCycle;
        friend Collider2D;
        friend physics::Collider;
    protected:
        Component();
        bool wasStartCalled() { return StartCalled; }
    private:
        struct AddComponentDesc
        {
#if _DEBUG
            bool addAvailable;
#endif
            GameObject* gameObject;
            GUID guid;
        };
        std::weak_ptr<Component> GetWeakPtrFromGameObject()const;
        static Component::AddComponentDesc addComponentDesc;
        static std::unordered_map<GUID, Component*, yutility::GenericHash<GUID>, yutility::GenericEqual<GUID>> guidPtrMap;
        std::unordered_set<std::shared_ptr<coroutine::Coroutine>> coroutines;
        GameObject* gameObject = nullptr;
        bool StartCalled = false;
        bool isActive{ true };
        // 업데이트 함수가 오버라이드되어 업데이트를 부르는 것에 의미가 있는지를 나타냅니다.
        // 업데이트 함수의 본문이 비어있을 경우 최적화를 위해 updatable의 값은 false가 되고, 게임 사이클에서 제외됩니다.
        bool isUpdating{ true };
        GUID guid;
    };
}
