#pragma once
#include <assert.h>
#include "YunutyEngine.h"
#include "YunutyForceType.h"
#include "YunutyCollider.h"
#include "PxPhysicsAPI.h"
#include "_YunutyPhysxGlobal.h"

using namespace std;
using namespace physx;
namespace yunutyEngine
{
	namespace physics
	{
		// 이 클래스의 설정값만 건드리면 콜라이더가 물리 강체, 트리거 볼륨의 기능을 모두 수행할 수 있지만 콘텐츠 개발자 입장에서는 이 클래스의 존재를 알 수 없다.
		// 이 객체는 Collider 컴포넌트 안에서 정의되지만, 이 객체의 값들을 건드리는 인터페이스는 Rigidbody Component와 Collider Component에 존재합니다.
		class Collider::Impl
		{
		private:
			Collider* colliderComponent;
			physx::PxRigidStatic* pxRigidStatic{ nullptr };
			physx::PxRigidDynamic* pxRigidDynamic{ nullptr };
			physx::PxShape* pxShape{ nullptr };
			physx::PxMaterial* pxMaterial{ nullptr };
			physx::PxRigidActor* pxActor{ nullptr };
			PxShapeFlags pxShapeFlags{ PxShapeFlag::eVISUALIZATION | physx::PxShapeFlag::eTRIGGER_SHAPE };
			bool isStatic{ false };
			bool isKinematic{ false };
			bool ccdEnabled{ false };
			friend Collider;
			friend RigidBody;
			friend _PhysxGlobal;

		protected:
		public:
			// Rigidbody가 존재하지 않으면 isTrigger는 참, Rigidbody가 존재하면 Trigger는 거짓입니다.
			/*bool GetIsTrigger()
			{
				return pxShapeFlags.isSet(physx::PxShapeFlag::eTRIGGER_SHAPE);
			}
			void SetAsTrigger(bool isTrigger)
			{
				if (isTrigger)
					pxShapeFlags.raise(physx::PxShapeFlag::eTRIGGER_SHAPE);
				else
					pxShapeFlags.clear(physx::PxShapeFlag::eTRIGGER_SHAPE);

				if (pxShape)
					pxShape->setFlags(pxShapeFlags);
			}*/

			Impl(Collider* colliderComponent)
			{
				this->colliderComponent = colliderComponent;
			}
			bool isRigidBody()
			{
				return pxShapeFlags.isSet(physx::PxShapeFlag::eSIMULATION_SHAPE);
			}
			void SetAsRigidbody(RigidBody* rigidBody)
			{
				isStatic = false;
				colliderComponent->rigidBody = rigidBody;
				if (rigidBody != nullptr)
				{
					pxShapeFlags.clear(physx::PxShapeFlag::eTRIGGER_SHAPE);
					pxShapeFlags.raise(physx::PxShapeFlag::eSIMULATION_SHAPE);
				}
				else
				{
					pxShapeFlags.clear(physx::PxShapeFlag::eSIMULATION_SHAPE);
					pxShapeFlags.raise(physx::PxShapeFlag::eTRIGGER_SHAPE);
				}

				if (pxShape)
					pxShape->setFlags(pxShapeFlags);
			}
			bool IsUsingCCD()
			{
				return ccdEnabled;
			}
			void EnableCCD(bool enable)
			{
				ccdEnabled = enable;
				if (pxRigidDynamic)
					pxRigidDynamic->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, enable);
			}
			bool isStaticShape()
			{
				return isStatic;
			}
			void SetAsStatic(bool isStatic)
			{
				// 한번 스태틱으로 지정한 RigidBody는 다시 바꾸지 마시오. 그런 행동을 시도하면 아래 assert 문에서 런타임 오류가 잡히게 됩니다.
				// 정 그런 구성이 필요하다면 게임 엔진 개발자에게 문의하시오.
				assert((pxRigidStatic == nullptr && pxRigidDynamic == nullptr) || this->isStatic == isStatic);
				this->isStatic = isStatic;
			}
			void SetAsKinematic(bool isKinematic)
			{
				this->isKinematic = isKinematic;
				if (pxRigidDynamic)
					pxRigidDynamic->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, isKinematic);
			}
			bool IsKinematic()
			{
				return isKinematic;
			}
			virtual ~Impl()
			{
				if (pxActor)
				{
					_PhysxGlobal::SingleInstance().UnRegisterCollider(this);
					pxActor->release();
				}
			}
			virtual PxGeometry& GetGeometry() = 0;
			// 게임 컴포넌트에 대응되는 physX 객체들을 초기화하는 구문이다. Start함수에서 호출된다.
			void InitializePhysXActor()
			{
				static_assert(sizeof(yunuGI::Matrix4x4) == sizeof(PxMat44));
				auto worldTM = colliderComponent->GetTransform()->GetWorldTM();
				pxMaterial = _PhysxGlobal::SingleInstance().pxPhysics->createMaterial(1.0f, 1.0f, 0.0f);

				pxShape = _PhysxGlobal::SingleInstance().pxPhysics->createShape(GetGeometry(), *pxMaterial, true, pxShapeFlags);

				if (isStatic)
				{
					pxRigidStatic = _PhysxGlobal::SingleInstance().pxPhysics->createRigidStatic(PxTransform(reinterpret_cast<const PxMat44&>(worldTM)));
					_PhysxGlobal::SingleInstance().RequestPxScene(colliderComponent->GetGameObject()->GetScene())->addActor(*pxRigidStatic);
					pxRigidStatic->attachShape(*pxShape);
					pxActor = pxRigidStatic;
				}
				else
				{
					//assert(PxTransform{ PxMat44{ reinterpret_cast<const PxMat44&>(worldTM) } }.isValid(), "월드 스케일이 1이여야 됨 ㅋ");
					pxRigidDynamic = _PhysxGlobal::SingleInstance().pxPhysics->createRigidDynamic(PxTransform(PxMat44{ reinterpret_cast<const PxMat44&>(worldTM) }));
					_PhysxGlobal::SingleInstance().RequestPxScene(colliderComponent->GetGameObject()->GetScene())->addActor(*pxRigidDynamic);
					pxRigidDynamic->attachShape(*pxShape);
					if (ccdEnabled)
						pxRigidDynamic->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
					if (colliderComponent->rigidBody == nullptr)
						isKinematic = true;
					if (isKinematic)
						pxRigidDynamic->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
					pxActor = pxRigidDynamic;
				}
				_PhysxGlobal::SingleInstance().RegisterCollider(this);
				pxShape->release();
			}
			void LockTranslation(bool x, bool y, bool z)
			{
				if (isStatic)
					return;

				pxRigidDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_X, x);
				pxRigidDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, y);
				pxRigidDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, z);
			}
			void LockRotation(bool x, bool y, bool z)
			{
				if (isStatic)
					return;

				pxRigidDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, x);
				pxRigidDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, y);
				pxRigidDynamic->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, z);
			}
			void SetMass(float mass)
			{
				if (isStatic)
					return;

				pxRigidDynamic->setMass(mass);
			}
			void AddForce(const Vector3f& forceVector, ForceType forceType)
			{
				if (isStatic)
					return;

				static_assert((int)ForceType::FORCE == (int)PxForceMode::eFORCE);
				static_assert((int)ForceType::IMPULSE == (int)PxForceMode::eIMPULSE);
				static_assert((int)ForceType::VELOCITY_CHANGE == (int)PxForceMode::eVELOCITY_CHANGE);
				static_assert((int)ForceType::ACCELERATION == (int)PxForceMode::eACCELERATION);

				pxRigidDynamic->addForce(reinterpret_cast<const PxVec3&>(forceVector), reinterpret_cast<const PxForceMode::Enum&>(forceType));
			}
			void AddTorque(const Vector3f& forceVector, ForceType torqueType)
			{
				if (isStatic)
					return;

				static_assert((int)ForceType::FORCE == (int)PxForceMode::eFORCE);
				static_assert((int)ForceType::IMPULSE == (int)PxForceMode::eIMPULSE);
				static_assert((int)ForceType::VELOCITY_CHANGE == (int)PxForceMode::eVELOCITY_CHANGE);
				static_assert((int)ForceType::ACCELERATION == (int)PxForceMode::eACCELERATION);

				pxRigidDynamic->addTorque(reinterpret_cast<const PxVec3&>(forceVector), reinterpret_cast<const PxForceMode::Enum&>(torqueType));
			}
			void SetActorWorldTransform(const yunuGI::Matrix4x4& worldTM)
			{
				static_assert(sizeof(yunuGI::Matrix4x4) == sizeof(PxMat44));
				pxActor->setGlobalPose(PxTransform{ reinterpret_cast<const PxMat44&>(worldTM) });
			}
			yunutyEngine::Vector3f GetActorPosition()
			{
				static_assert(sizeof(Vector3f) == sizeof(PxVec3));
				return reinterpret_cast<const Vector3f&>(pxActor->getGlobalPose().p);
			}
			yunutyEngine::Quaternion GetActorRotation()
			{
				static_assert(sizeof(Quaternion) == sizeof(PxQuatd));

				auto pxQuat = pxActor->getGlobalPose().q;
				yunutyEngine::Quaternion quat{ pxQuat.w, pxQuat.x, pxQuat.y, pxQuat.z };
				return quat;
			}
		};
	}
}
