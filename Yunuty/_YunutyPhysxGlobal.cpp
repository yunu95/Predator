#include "_YunutyPhysxGlobal.h"
#include "_YunutyColliderImpl.h"

namespace yunutyEngine
{
    namespace physics
    {
        _PhysxGlobal::_PhysxGlobal()
        {
            pxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, pxAllocator, pxErrorCallback);
            pxPvd = PxCreatePvd(*pxFoundation);
            PxPvdTransport* transport{ PxDefaultPvdSocketTransportCreate("127.0.0.1",5425,10) };
            pxPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
            pxPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *pxFoundation, PxTolerancesScale(), true, pxPvd);
            PxInitExtensions(*pxPhysics, pxPvd);
            const PxU32 numCores = PxThread::getNbPhysicalCores();
            pxDispatcher = PxDefaultCpuDispatcherCreate(numCores == 0 ? 0 : numCores - 1);
        }
        _PhysxGlobal::~_PhysxGlobal()
        {
            for (auto each : PxSceneByScene)
                PX_RELEASE(each.second);

            PxCloseExtensions();
            PX_RELEASE(pxDispatcher);
            if (pxPvd)
            {
                auto transport = pxPvd->getTransport();
                PX_RELEASE(pxPvd);
                PX_RELEASE(transport);
            }
            PX_RELEASE(pxFoundation);
        }
        void _PhysxGlobal::RegisterCollider(Collider::Impl* impl)
        {
            ColliderByPxActor[impl->pxActor] = impl->colliderComponent;
        }
        void _PhysxGlobal::UnRegisterCollider(Collider::Impl* impl)
        {
            ColliderByPxActor.erase(impl->pxActor);
        }
    }
}
