#pragma once
#include "PxPhysicsAPI.h"

using namespace std;
using namespace physx;
namespace yunutyEngine
{
    namespace physics
    {
        static	PxFilterFlags triggersUsingFilterCallback(PxFilterObjectAttributes /*attributes0*/, PxFilterData /*filterData0*/,
            PxFilterObjectAttributes /*attributes1*/, PxFilterData /*filterData1*/,
            PxPairFlags& pairFlags, const void* /*constantBlock*/, PxU32 /*constantBlockSize*/)
        {
            //	printf("contactReportFilterShader\n");

            PX_ASSERT(getImpl() == FILTER_CALLBACK);

            //pairFlags = PxPairFlag::eCONTACT_DEFAULT;

            //if (usesCCD())
                //pairFlags |= PxPairFlag::eDETECT_CCD_CONTACT | PxPairFlag::eNOTIFY_TOUCH_CCD;

            return PxFilterFlag::eCALLBACK;
        }
        class ContactReportCallback : public PxSimulationEventCallback
        {
            void onConstraintBreak(PxConstraintInfo* /*constraints*/, PxU32 /*count*/)	PX_OVERRIDE;
            void onWake(PxActor** /*actors*/, PxU32 /*count*/)	PX_OVERRIDE;
            void onSleep(PxActor** /*actors*/, PxU32 /*count*/)	PX_OVERRIDE;
            void onTrigger(PxTriggerPair* pairs, PxU32 count)	PX_OVERRIDE;
            void onAdvance(const PxRigidBody* const*, const PxTransform*, const PxU32)	PX_OVERRIDE;
            void onContact(const PxContactPairHeader& /*pairHeader*/, const PxContactPair* pairs, PxU32 count)	PX_OVERRIDE;
        };
        class TriggersFilterCallback : public PxSimulationFilterCallback
        {
            virtual		PxFilterFlags	pairFound(PxU64 /*pairID*/,
                PxFilterObjectAttributes /*attributes0*/, PxFilterData /*filterData0*/, const PxActor* /*a0*/, const PxShape* s0,
                PxFilterObjectAttributes /*attributes1*/, PxFilterData /*filterData1*/, const PxActor* /*a1*/, const PxShape* s1,
                PxPairFlags& pairFlags)	PX_OVERRIDE
            {
                //		printf("pairFound\n");

                if (s0->userData || s1->userData)	// See createTriggerShape() function
                {
                    pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
                }
                else
                    pairFlags = PxPairFlag::eCONTACT_DEFAULT;

                return PxFilterFlags();
            }

            virtual		void	pairLost(PxU64 /*pairID*/,
                PxFilterObjectAttributes /*attributes0*/, PxFilterData /*filterData0*/,
                PxFilterObjectAttributes /*attributes1*/, PxFilterData /*filterData1*/,
                bool /*objectRemoved*/)	PX_OVERRIDE
            {
                //		printf("pairLost\n");
            }

            virtual		bool	statusChange(PxU64& /*pairID*/, PxPairFlags& /*pairFlags*/, PxFilterFlags& /*filterFlags*/)	PX_OVERRIDE
            {
                //		printf("statusChange\n");
                return false;
            }
        }gTriggersFilterCallback;
    }
}
