#include "YunutyEngine.h"
#include "YunutyNavigationField.h"
#include "YunutyNavigationAgent.h"
#include "_YunutyNavigationAgentImpl.h"
#include "_YunutyNavigationFieldImpl.h"

using namespace yunutyEngine;

yunutyEngine::NavigationAgent::NavigationAgent() : impl(new Impl{ this })
{ }
yunutyEngine::NavigationAgent::~NavigationAgent()
{
    AssignToNavigationField(nullptr);
    delete impl;
}
void yunutyEngine::NavigationAgent::Update()
{
    if (impl->crowd == nullptr)
        return;

    const dtCrowdAgent* agent = impl->crowd->getAgent(impl->agentIdx);
    constexpr float deltaY = -0.81f;
    GetTransform()->SetWorldPosition(Vector3f(agent->npos[0], agent->npos[1] + deltaY, agent->npos[2]));
}
void yunutyEngine::NavigationAgent::DetachFromNavigationField()
{
    AssignToNavigationField(nullptr);
}
void yunutyEngine::NavigationAgent::OnEnable()
{
    if (navField != nullptr)
    {
        AssignToNavigationField(navField);
        //if (impl->agentIdx != -1)
        //{
        //    impl->crowd->removeAgent(impl->agentIdx);
        //    impl->agentIdx = -1;
        //}
        //navField->agents.insert(this);
        //Vector3f pos = GetTransform()->GetWorldPosition();
        //impl->agentIdx = impl->crowd->addAgent(reinterpret_cast<float*>(&pos), &impl->agentParams);
        //Relocate(pos);
    }
}
void yunutyEngine::NavigationAgent::OnDisable()
{
    if (navField != nullptr)
    {
        impl->crowd->removeAgent(impl->agentIdx);
        navField->agents.erase(this);
        impl->agentIdx = -1;
    }
}
void yunutyEngine::NavigationAgent::AssignToNavigationField(NavigationField* navField)
{
    if (impl->crowd != nullptr)
        impl->crowd->removeAgent(impl->agentIdx);

    if (this->navField != nullptr)
        this->navField->agents.erase(this);

    if (navField != nullptr)
    {
        navField->agents.insert(this);
        Vector3f pos = GetTransform()->GetWorldPosition();
        impl->crowd = navField->impl->m_crowd;
        impl->agentIdx = impl->crowd->addAgent(reinterpret_cast<float*>(&pos), &impl->agentParams);
        //auto agent = impl->crowd->getAgent(impl->agentIdx);
    }
    this->navField = navField;
}
NavigationField* yunutyEngine::NavigationAgent::GetAssignedNavigationField()
{
    return navField;
}
void yunutyEngine::NavigationAgent::SetSpeed(float speed)
{
    impl->agentParams.maxSpeed = speed;
    if (impl->crowd != nullptr)
        impl->crowd->updateAgentParameters(impl->agentIdx, &impl->agentParams);
}
void yunutyEngine::NavigationAgent::SetAcceleration(float accel)
{
    impl->agentParams.maxAcceleration = accel;
    if (impl->crowd != nullptr)
        impl->crowd->updateAgentParameters(impl->agentIdx, &impl->agentParams);
}
void yunutyEngine::NavigationAgent::SetRadius(float radius)
{
    impl->agentParams.radius = radius;
    if (impl->crowd != nullptr)
        impl->crowd->updateAgentParameters(impl->agentIdx, &impl->agentParams);
}
float yunutyEngine::NavigationAgent::GetSpeed()
{
    return impl->agentParams.maxSpeed;
}
float yunutyEngine::NavigationAgent::GetAcceleration()
{
    return impl->agentParams.maxAcceleration;
}
float yunutyEngine::NavigationAgent::GetRadius()
{
    return impl->agentParams.radius;
}
void yunutyEngine::NavigationAgent::Relocate(Vector3f destination)
{
    if (impl->agentIdx == -1)
    {
        GetTransform()->SetWorldPosition(destination);
        return;
    }
    const float* pos = &destination.x;
    constexpr float agentPlacementHalfExtents[3]{ 1000,1000,1000 };
    float nearest[3];
    dtPolyRef ref = 0;
    dtVcopy(nearest, pos);

    dtStatus status = navField->impl->m_navQuery->findNearestPoly(pos, agentPlacementHalfExtents, impl->crowd->getFilter(0), &ref, nearest);
    if (dtStatusFailed(status))
    {
        dtVcopy(nearest, pos);
        ref = 0;
    }
    dtCrowdAgent* ag = impl->crowd->getEditableAgent(impl->agentIdx);
    ag->corridor.reset(ref, nearest);
    ag->boundary.reset();
    ag->partial = false;

    ag->topologyOptTime = 0;
    ag->targetReplanTime = 0;
    ag->nneis = 0;

    dtVset(ag->dvel, 0, 0, 0);
    dtVset(ag->nvel, 0, 0, 0);
    dtVset(ag->vel, 0, 0, 0);
    dtVcopy(ag->npos, nearest);

    ag->desiredSpeed = 0;

    if (ref)
        ag->state = DT_CROWDAGENT_STATE_WALKING;
    else
        ag->state = DT_CROWDAGENT_STATE_INVALID;

    ag->targetState = DT_CROWDAGENT_TARGET_NONE;
    Update();
}
void yunutyEngine::NavigationAgent::MoveTo(Vector3f destination)
{
    if (navField == nullptr)
        return;
    const dtQueryFilter* filter{ impl->crowd->getFilter(0) };
    const dtCrowdAgent* agent = impl->crowd->getAgent(impl->agentIdx);
    static constexpr float halfExtents[]{ 100,100,100 };
    //const float* halfExtents = impl->crowd->getQueryExtents();

    navField->impl->m_navQuery->findNearestPoly(reinterpret_cast<float*>(&destination), halfExtents, filter, &impl->targetRef, impl->targetPos);
    //assert(!(impl->targetRef == 0 && impl->targetPos[0] == 0 && impl->targetPos[1] == 0 && impl->targetPos[2] == 0)
    //	&& "네비게이션 쿼리가 제대로 된 도착지점을 계산하지 못했습니다!");
    impl->crowd->requestMoveTarget(impl->agentIdx, impl->targetRef, impl->targetPos);
}
const Vector3f& yunutyEngine::NavigationAgent::GetTargetPosition()
{
    return reinterpret_cast<const Vector3f&>(*impl->targetPos);
}
