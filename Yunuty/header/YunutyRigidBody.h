#pragma once
#include <Windows.h>
#include <vector>
#include <unordered_set>
#include "Graphic.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Rect.h"
#include "YunutyForceType.h"
#include "YunutyPhysicsMaterialInfo.h"

#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif

// RigidBody는 콜라이더에 강체 속성을 부여하고 싶을때 사용됩니다. 콜라이더 없이는 RigidBody의 코드가 제대로 동작할 수 없습니다.
using namespace std;
namespace yunutyEngine
{
    namespace physics
    {
        class Collider;
        class YUNUTY_API RigidBody : virtual public Component
        {
        public:
            RigidBody();
            virtual ~RigidBody();
            bool GetIsStatic();
            void SetAsStatic(bool isStatic);
            void SetAsKinematic(bool isKinematic);
            bool GetIsKinematic();

            //void SetMaterialInfo();

            void LockTranslation(bool x, bool y, bool z);
            void LockRotation(bool x, bool y, bool z);
            void SetMass(float mass);
            void AddForce(const Vector3f& forceVector, ForceType forceType);
            void AddTorque(const Vector3f& forceVector, ForceType torqueType);
        protected:
            virtual void Start() override;
            virtual void OnEnable()override;
            virtual void OnDisable()override;
        private:
            std::vector<std::function<void()>> onStartCallbacks;
            Collider* attachedCollider;
            bool isStatic{ false };
            friend Collider;
        };
    }
}