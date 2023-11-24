#include "YunutyEngine.h"
#include "_YunutyPhysxContactCallback.h"
#include "_YunutyPhysxGlobal.h"
void yunutyEngine::physics::ContactReportCallback::onConstraintBreak(PxConstraintInfo* /*constraints*/, PxU32 /*count*/)
{
    printf("onConstraintBreak\n");
}

void yunutyEngine::physics::ContactReportCallback::onWake(PxActor** /*actors*/, PxU32 /*count*/)
{
    printf("onWake\n");
}

void yunutyEngine::physics::ContactReportCallback::onSleep(PxActor** /*actors*/, PxU32 /*count*/)
{
    printf("onSleep\n");
}

void yunutyEngine::physics::ContactReportCallback::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
    //		printf("onTrigger: %d trigger pairs\n", count);
    while (count--)
    {
        const PxTriggerPair& current = *pairs++;
        auto triggerCollider = _PhysxGlobal::SingleInstance().ColliderByPxActor[current.triggerActor];
        auto otherCollider = _PhysxGlobal::SingleInstance().ColliderByPxActor[current.otherActor];
        void (Component:: * callback)(const Collider*) { nullptr };
        //if (current.status & PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
            //callback = &Component::OnTriggerStay;
        //else
        if (current.status & PxPairFlag::eNOTIFY_TOUCH_FOUND)
            callback = &Component::OnTriggerEnter;
        else if (current.status & PxPairFlag::eNOTIFY_TOUCH_LOST)
            callback = &Component::OnTriggerExit;
        else
            continue;
        //if (current.status & PxPairFlag::enotify_touch)
            //printf("Shape is entering trigger volume\n");
        for (Component* eachComponent : triggerCollider->GetGameObject()->GetComponents<Component>())
        {
            (eachComponent->*callback)(otherCollider);
        };
        for (auto eachComponent : otherCollider->GetGameObject()->GetComponents<Component>())
        {
            (eachComponent->*callback)(triggerCollider);
        };
        if (current.status & PxPairFlag::eNOTIFY_TOUCH_FOUND)
            printf("Shape is entering trigger volume\n");
        if (current.status & PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
            printf("Shape is persisting\n");
        if (current.status & PxPairFlag::eNOTIFY_TOUCH_LOST)
            printf("Shape is leaving trigger volume\n");
    }
}

void yunutyEngine::physics::ContactReportCallback::onAdvance(const PxRigidBody* const*, const PxTransform*, const PxU32)
{
    printf("onAdvance\n");
}

void yunutyEngine::physics::ContactReportCallback::onContact(const PxContactPairHeader& /*pairHeader*/, const PxContactPair* pairs, PxU32 count)
{
    //		printf("onContact: %d pairs\n", count);

    while (count--)
    {
        const PxContactPair& current = *pairs++;

        // The reported pairs can be trigger pairs or not. We only enabled contact reports for
        // trigger pairs in the filter shader, so we don't need to do further checks here. In a
        // real-world scenario you would probably need a way to tell whether one of the shapes
        // is a trigger or not. You could e.g. reuse the PxFilterData like we did in the filter
        // shader, or maybe use the shape's userData to identify triggers, or maybe put triggers
        // in a hash-set and test the reported shape pointers against it. Many options here.

        if (current.events & (PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_TOUCH_CCD))
            printf("Shape is entering trigger volume\n");
        if (current.events & PxPairFlag::eNOTIFY_TOUCH_LOST)
            printf("Shape is leaving trigger volume\n");

        //if (isTriggerShape(current.shapes[0]) && isTriggerShape(current.shapes[1]))
            //printf("Trigger-trigger overlap detected\n");
    }
}
