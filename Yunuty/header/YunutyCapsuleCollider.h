#pragma once
#include <Windows.h>
#include <vector>
#include <unordered_set>
#include "YunutyCollider.h"
#include "Component.h"
#include "Vector2.h"
#include "Vector3.h"

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
        class YUNUTY_API CapsuleCollider : public Collider
        {
        public:
            void SetRadius(float radius) {};
            void SetHeight(float height) {};
            virtual void Start()override {};
        };
    }
}
