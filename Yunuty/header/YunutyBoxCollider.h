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
        class YUNUTY_API BoxCollider : public Collider
        {
        public:
            BoxCollider();
            class Impl;
            Impl* impl;

            void SetHalfExtent(const Vector3f& halfExtent);
            virtual void ApplyScale(const Vector3d& halfExtent) override;
        private:
            Vector3f unscaledHalfExtent;
        };
    }
}
