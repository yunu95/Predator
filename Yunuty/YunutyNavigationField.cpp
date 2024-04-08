#include "_YunutyNavigationFieldImpl.h"
#include "YunutyNavigationField.h"
#include "DetourStatus.h"
#include "DetourNavMeshQuery.h"
#include "fastlz.h"

using namespace yunutyEngine;

yunutyEngine::NavigationField::NavigationField()
{
    impl = new Impl(this);
}
yunutyEngine::NavigationField::~NavigationField()
{
    for (auto each : agents)
        each->AssignToNavigationField(nullptr);
    for (auto each : obstacles)
        each->AssignToNavigationField(nullptr);
    delete impl;
}
void yunutyEngine::NavigationField::Update()
{
    if (!IsInitialized())
        return;
    impl->m_tileCache->update(yunutyEngine::Time::GetDeltaTime(), impl->m_navMesh, nullptr);
    impl->m_crowd->update(yunutyEngine::Time::GetDeltaTime(), nullptr);
}
Vector3d yunutyEngine::NavigationField::GetClosestPointOnField(const Vector3d& worldPosition) const
{
    float pos[3]{ worldPosition.x,worldPosition.y,worldPosition.z };
    float halfExtent[3]{ 100,100,100 };
    dtPolyRef ref = 0;
    dtStatus status = impl->m_navQuery->findNearestPoly(pos, halfExtent, impl->m_crowd->getFilter(0), &ref, pos);
    return Vector3d{ Vector3f{pos[0],pos[1],pos[2]} };
}
void yunutyEngine::NavigationField::BuildField(const float* worldVertices, size_t verticesNum, const int* faces, size_t facesNum, const BuildSettings& buildSettings)
{
    if (IsInitialized())
        CleanUpField();

    auto success = impl->handleBuild(worldVertices, verticesNum, faces, facesNum, buildSettings);
    if (success)
    {
        auto tempAgents = agents;
        auto tempObstacles = obstacles;
        for (auto each : tempAgents)
            each->AssignToNavigationField(this);
        for (auto each : tempObstacles)
            each->AssignToNavigationField(this);
    }

    assert(success);
};

bool yunutyEngine::NavigationField::IsInitialized()
{
    return impl->m_navMesh != nullptr;
}
void yunutyEngine::NavigationField::CleanUpField()
{
    //if (impl->m_polyMesh)
    //{
    //    rcFreePolyMesh(impl->m_polyMesh);
    //    impl->m_polyMesh = nullptr;
    //}
    //if (impl->m_polyMeshDetail)
    //{
    //    rcFreePolyMeshDetail(impl->m_polyMeshDetail);
    //    impl->m_polyMeshDetail = nullptr;
    //}
    if (impl->m_navMesh)
    {
        dtFreeNavMesh(impl->m_navMesh);
        impl->m_navMesh = nullptr;
    }
    for (int i = 0; impl->m_tileCache && i < impl->m_tileCache->getObstacleCount(); ++i)
    {
        const dtTileCacheObstacle* ob = impl->m_tileCache->getObstacle(i);
        if (ob->state == DT_OBSTACLE_EMPTY) continue;
        impl->m_tileCache->removeObstacle(impl->m_tileCache->getObstacleRef(ob));
    }
    if (impl->m_tileCache)
    {
        dtFreeTileCache(impl->m_tileCache);
        impl->m_tileCache = nullptr;
    }
}
