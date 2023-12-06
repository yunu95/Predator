#pragma once
#include <Windows.h>
#include <vector>
#include <unordered_set>
#include "YunutyCollider.h"
#include "Component.h"
#include "Vector2.h"
#include "Vector3.h"
#include "SingletonClass.h"
#include "PxPhysicsAPI.h"
#include "_YunutyPhysxContactCallback.h"
//#include "_YunutyColliderImpl.h"
#include "Scene.h"
#include <map>


using namespace std;
using namespace physx;
namespace yunutyEngine
{
    namespace physics
    {
        class Collider::Impl;
        class _PhysxGlobal : public SingletonClass<_PhysxGlobal>
        {
        public:
            _PhysxGlobal();

            PxFoundation* pxFoundation;
            PxPvd* pxPvd;
            PxPhysics* pxPhysics;

            // pxScene이 없는 경우 PxScene을 만들어서 준다.
            PxScene* RequestPxScene(Scene* scene)
            {
                if (auto itr = PxSceneByScene.find(scene); itr == PxSceneByScene.end())
                {
                    PxSceneDesc sceneDesc(pxPhysics->getTolerancesScale());
                    sceneDesc.cpuDispatcher = pxDispatcher;
                    sceneDesc.gravity = PxVec3{ 0,-9.81f,0 };
                    sceneDesc.simulationEventCallback = &pxContactReportCallback;
                    sceneDesc.filterShader = PxDefaultSimulationFilterShader;
                    auto pxScene = PxSceneByScene[scene] = pxPhysics->createScene(sceneDesc);
                    if (scene == Scene::getCurrentScene())
                    {
                        PxPvdSceneClient* pvdClient = pxScene->getScenePvdClient();
                        if (pvdClient)
                            pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
                    }
                    return pxScene;
                }
                else
                    return itr->second;
            };
            void RegisterCollider(Collider::Impl* impl);
            void UnRegisterCollider(Collider::Impl* impl);
        private:
            ~_PhysxGlobal();
            PxDefaultAllocator pxAllocator;
            PxDefaultErrorCallback	pxErrorCallback;
            ContactReportCallback pxContactReportCallback;
            TriggersFilterCallback pxTriggersFilterCallback;
            PxDefaultCpuDispatcher* pxDispatcher;
            map<Scene*, PxScene*> PxSceneByScene;
            map<Scene*, PxPvdSceneClient*> PvdByScene;
            map<PxActor*, Collider*> ColliderByPxActor;
            friend YunutyCycle;
            friend ContactReportCallback;
        };
    }
}
