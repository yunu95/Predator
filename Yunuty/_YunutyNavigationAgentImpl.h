#pragma once
#include "DetourCrowd.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshQuery.h"
#include "DetourObstacleAvoidance.h"
#include "DetourCommon.h"
#include "DetourMath.h"
#include "DetourAssert.h"
#include "DetourAlloc.h"
#include "YunutyNavigationAgent.h"

namespace yunutyEngine
{
    class NavigationAgent::Impl
    {
    private:
        Impl(NavigationAgent* navAgentComponent) :navAgentComponent(navAgentComponent)
        {
            navAgentComponent = navAgentComponent;
        }
        virtual ~Impl()
        {
            if (crowd != nullptr && agentIdx != -1)
                crowd->removeAgent(agentIdx);
        }
        friend NavigationAgent;
    public:
        int agentIdx{-1};
        //const dtCrowdAgent* agent{ nullptr };
        dtCrowd* crowd{ nullptr };
        dtPolyRef targetRef;
        float targetPos[3];
        dtCrowdAgentParams agentParams
        {
            .radius = 0.5,
            .height = 0.01,
            .maxAcceleration = std::numeric_limits<float>::max(),
            .maxSpeed = 5,
            .collisionQueryRange = 12,
            .pathOptimizationRange = 30,
            .separationWeight = 2,
            .updateFlags = DT_CROWD_OPTIMIZE_TOPO |
            DT_CROWD_OPTIMIZE_VIS |
            DT_CROWD_OBSTACLE_AVOIDANCE,
            .obstacleAvoidanceType = (unsigned char)3,
        };
        NavigationAgent* navAgentComponent;
    };
}
