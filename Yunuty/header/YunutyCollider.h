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
			//bool IsTrigger();
			//void SetAsTrigger(bool isTrigger);
			bool IsUsingCCD();
			// Continuous Collision Detection 기능을 활성화하면 한 프레임에서 다음 프레임까지의 충돌을 연속적으로 체크합니다.
			// CCD 기능이 활성화되면 Bullet through paper 현상이 발생하지 않습니다.
			void EnableCCD(bool enable);
			// 콜라이더의 크기가 0이면 터짐, 콜라이더 크기가 매우 작을때 적용되는 최소한의 스케일
		protected:
			virtual void Start()override;
			virtual void Update()override;
			// 피직스 객체의 월드스케일이 달라졌을 때 이를 어떻게 반영할지 결정합니다.
			virtual void ApplyScale(const Vector3d& worldScale) = 0;
			virtual void OnEnable()override;
			virtual void OnDisable()override;
			RigidBody* rigidBody;

			Impl* impl;
		private:
			bool WasPxActorInitialized();
			Vector3d cachedScale;
			friend RigidBody;
#ifdef _DEBUG
			Vector3d firstLocation;
			Quaternion firstRotation;
#endif
		};
	}
}
