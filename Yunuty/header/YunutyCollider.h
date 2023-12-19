#pragma once
#include <Windows.h>
#include <vector>
#include <unordered_set>
#include "Graphic.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Rect.h"

#ifdef YUNUTY_EXPORTS
#define YUNUTY_API __declspec(dllexport)
#else
#define YUNUTY_API __declspec(dllimport)
#endif

using namespace std;
namespace yunutyEngine
{
    namespace physics
    {
        class RigidBody;
        class YUNUTY_API Collider : public Component
        {
        public:
            class Impl;
            Collider(Impl* impl);
            virtual ~Collider();
        protected:
            virtual void Start() override;
            virtual void Update()override;
            virtual void OnEnable()override;
            virtual void OnDisable()override;
            RigidBody* rigidBody;

            Impl* impl;
        public:
            //bool IsTrigger();
            //void SetAsTrigger(bool isTrigger);
            bool IsUsingCCD();
            // Continuous Collision Detection 기능을 활성화하면 한 프레임에서 다음 프레임까지의 충돌을 연속적으로 체크합니다.
            // CCD 기능이 활성화되면 Bullet through paper 현상이 발생하지 않습니다.
            void EnableCCD(bool enable);
        private:
            friend RigidBody;
#ifdef _DEBUG
            Vector3d firstLocation;
            Quaternion firstRotation;
#endif
        };
    }
}
