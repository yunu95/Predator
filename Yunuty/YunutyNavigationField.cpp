#include "_YunutyNavigationFieldImpl.h"
#include "YunutyNavigationField.h"
#include "DetourStatus.h"
#include "DetourNavMeshQuery.h"
#include "fastlz.h"

using namespace yunutyEngine;

inline bool inRange(const float* v1, const float* v2, const float r, const float h);
static bool getSteerTarget(dtNavMeshQuery* navQuery, const float* startPos, const float* endPos,
    const float minTargetDist,
    const dtPolyRef* path, const int pathSize,
    float* steerPos, unsigned char& steerPosFlag, dtPolyRef& steerPosRef,
    float* outPoints = 0, int* outPointCount = 0)
{
    // Find steer target.
    static const int MAX_STEER_POINTS = 3;
    float steerPath[MAX_STEER_POINTS * 3];
    unsigned char steerPathFlags[MAX_STEER_POINTS];
    dtPolyRef steerPathPolys[MAX_STEER_POINTS];
    int nsteerPath = 0;
    navQuery->findStraightPath(startPos, endPos, path, pathSize,
        steerPath, steerPathFlags, steerPathPolys, &nsteerPath, MAX_STEER_POINTS);
    if (!nsteerPath)
        return false;

    if (outPoints && outPointCount)
    {
        *outPointCount = nsteerPath;
        for (int i = 0; i < nsteerPath; ++i)
            dtVcopy(&outPoints[i * 3], &steerPath[i * 3]);
    }


    // Find vertex far enough to steer to.
    int ns = 0;
    while (ns < nsteerPath)
    {
        // Stop at Off-Mesh link or when point is further than slop away.
        if ((steerPathFlags[ns] & DT_STRAIGHTPATH_OFFMESH_CONNECTION) ||
            !inRange(&steerPath[ns * 3], startPos, minTargetDist, 1000.0f))
            break;
        ns++;
    }
    // Failed to find good point to steer to.
    if (ns >= nsteerPath)
        return false;

    dtVcopy(steerPos, &steerPath[ns * 3]);
    steerPos[1] = startPos[1];
    steerPosFlag = steerPathFlags[ns];
    steerPosRef = steerPathPolys[ns];

    return true;
}
static int fixupCorridor(dtPolyRef* path, const int npath, const int maxPath,
    const dtPolyRef* visited, const int nvisited)
{
    int furthestPath = -1;
    int furthestVisited = -1;

    // Find furthest common polygon.
    for (int i = npath - 1; i >= 0; --i)
    {
        bool found = false;
        for (int j = nvisited - 1; j >= 0; --j)
        {
            if (path[i] == visited[j])
            {
                furthestPath = i;
                furthestVisited = j;
                found = true;
            }
        }
        if (found)
            break;
    }

    // If no intersection found just return current path. 
    if (furthestPath == -1 || furthestVisited == -1)
        return npath;

    // Concatenate paths.	

    // Adjust beginning of the buffer to include the visited.
    const int req = nvisited - furthestVisited;
    const int orig = rcMin(furthestPath + 1, npath);
    int size = rcMax(0, npath - orig);
    if (req + size > maxPath)
        size = maxPath - req;
    if (size)
        memmove(path + req, path + orig, size * sizeof(dtPolyRef));

    // Store visited
    for (int i = 0; i < req; ++i)
        path[i] = visited[(nvisited - 1) - i];

    return req + size;
}
static int fixupShortcuts(dtPolyRef* path, int npath, dtNavMeshQuery* navQuery)
{
    if (npath < 3)
        return npath;

    // Get connected polygons
    static const int maxNeis = 16;
    dtPolyRef neis[maxNeis];
    int nneis = 0;

    const dtMeshTile* tile = 0;
    const dtPoly* poly = 0;
    if (dtStatusFailed(navQuery->getAttachedNavMesh()->getTileAndPolyByRef(path[0], &tile, &poly)))
        return npath;

    for (unsigned int k = poly->firstLink; k != DT_NULL_LINK; k = tile->links[k].next)
    {
        const dtLink* link = &tile->links[k];
        if (link->ref != 0)
        {
            if (nneis < maxNeis)
                neis[nneis++] = link->ref;
        }
    }

    // If any of the neighbour polygons is within the next few polygons
    // in the path, short cut to that polygon directly.
    static const int maxLookAhead = 6;
    int cut = 0;
    for (int i = dtMin(maxLookAhead, npath) - 1; i > 1 && cut == 0; i--) {
        for (int j = 0; j < nneis; j++)
        {
            if (path[i] == neis[j]) {
                cut = i;
                break;
            }
        }
    }
    if (cut > 1)
    {
        int offset = cut - 1;
        npath -= offset;
        for (int i = 1; i < npath; i++)
            path[i] = path[i + offset];
    }

    return npath;
}
inline bool inRange(const float* v1, const float* v2, const float r, const float h)
{
    const float dx = v2[0] - v1[0];
    const float dy = v2[1] - v1[1];
    const float dz = v2[2] - v1[2];
    return (dx * dx + dz * dz) < r * r && fabsf(dy) < h;
}
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
    dtStatus status = impl->m_navQuery->findNearestPoly(pos, halfExtent, impl->m_crowd->getFilter(0), &impl->lastRef, pos);
    return Vector3d{ Vector3f{pos[0],pos[1],pos[2]} };
}
std::vector<Vector3d> yunutyEngine::NavigationField::GetSmoothPath(const Vector3d& start, const Vector3d& end)
{
    static constexpr int MAX_SMOOTH = 2048;
    static constexpr int MAX_POLYS = 256;
    static constexpr float STEP_SIZE = 0.5f;
    static constexpr float SLOP = 0.01f;
    std::vector<Vector3d> result;
    Vector3f startPos;
    Vector3f endPos;
    int m_npolys;
    dtPolyRef m_startRef;
    dtPolyRef m_endRef;
    dtPolyRef m_polys[MAX_POLYS];
    dtPolyRef m_parent[MAX_POLYS];
    dtQueryFilter m_filter;
    dtNavMeshQuery* m_navQuery{ impl->m_navQuery };
    int	m_nsmoothPath = 0;
    float iterPos[3], targetPos[3];
    float m_smoothPath[MAX_SMOOTH * 3];
    dtNavMesh* m_navMesh{ impl->m_navMesh };
    startPos = GetClosestPointOnField(start);
    m_startRef = impl->lastRef;
    endPos = GetClosestPointOnField(end);
    m_endRef = impl->lastRef;

    m_navQuery->findPath(m_startRef, m_endRef, reinterpret_cast<float*>(&startPos), reinterpret_cast<float*>(&endPos),
        &m_filter, m_polys, &m_npolys, MAX_POLYS);


    //dtPolyRef(&polys)[MAX_POLYS] = m_polys;
    dtPolyRef polys[MAX_POLYS];
    memcpy(polys, m_polys, sizeof(dtPolyRef) * m_npolys);
    int npolys = m_npolys;

    m_navQuery->closestPointOnPoly(m_startRef, reinterpret_cast<float*>(&startPos), iterPos, 0);
    m_navQuery->closestPointOnPoly(polys[npolys - 1], reinterpret_cast<float*>(&endPos), targetPos, 0);

    m_nsmoothPath = 0;

    dtVcopy(&m_smoothPath[m_nsmoothPath * 3], iterPos);
    m_nsmoothPath++;

    while (m_npolys && m_nsmoothPath < MAX_SMOOTH)
    {
        // Find location to steer towards.
        float steerPos[3];
        unsigned char steerPosFlag;
        dtPolyRef steerPosRef;

        if (!getSteerTarget(m_navQuery, iterPos, targetPos, SLOP,
            polys, npolys, steerPos, steerPosFlag, steerPosRef))
            break;

        bool endOfPath = (steerPosFlag & DT_STRAIGHTPATH_END) ? true : false;
        bool offMeshConnection = (steerPosFlag & DT_STRAIGHTPATH_OFFMESH_CONNECTION) ? true : false;

        // Find movement delta.
        float delta[3], len;
        dtVsub(delta, steerPos, iterPos);
        len = dtMathSqrtf(dtVdot(delta, delta));
        // If the steer target is end of path or off-mesh link, do not move past the location.
        if ((endOfPath || offMeshConnection) && len < STEP_SIZE)
            len = 1;
        else
            len = STEP_SIZE / len;
        float moveTgt[3];
        dtVmad(moveTgt, iterPos, delta, len);

        // Move
        float result[3];
        dtPolyRef visited[16];
        int nvisited = 0;
        m_navQuery->moveAlongSurface(polys[0], iterPos, moveTgt, &m_filter,
            result, visited, &nvisited, 16);

        npolys = fixupCorridor(polys, npolys, MAX_POLYS, visited, nvisited);
        npolys = fixupShortcuts(polys, npolys, m_navQuery);

        float h = 0;
        m_navQuery->getPolyHeight(polys[0], result, &h);
        result[1] = h;
        dtVcopy(iterPos, result);

        // Handle end of path and off-mesh links when close enough.
        if (endOfPath && inRange(iterPos, steerPos, SLOP, 1.0f))
        {
            // Reached end of path.
            dtVcopy(iterPos, targetPos);
            if (m_nsmoothPath < MAX_SMOOTH)
            {
                dtVcopy(&m_smoothPath[m_nsmoothPath * 3], iterPos);
                m_nsmoothPath++;
            }
            break;
        }
        else if (offMeshConnection && inRange(iterPos, steerPos, SLOP, 1.0f))
        {
            // Reached off-mesh connection.
            float startPos[3], endPos[3];

            // Advance the path up to and over the off-mesh connection.
            dtPolyRef prevRef = 0, polyRef = polys[0];
            int npos = 0;
            while (npos < npolys && polyRef != steerPosRef)
            {
                prevRef = polyRef;
                polyRef = polys[npos];
                npos++;
            }
            for (int i = npos; i < npolys; ++i)
                polys[i - npos] = polys[i];
            npolys -= npos;

            // Handle the connection.
            dtStatus status = m_navMesh->getOffMeshConnectionPolyEndPoints(prevRef, polyRef, startPos, endPos);
            if (dtStatusSucceed(status))
            {
                if (m_nsmoothPath < MAX_SMOOTH)
                {
                    dtVcopy(&m_smoothPath[m_nsmoothPath * 3], startPos);
                    m_nsmoothPath++;
                    // Hack to make the dotted path not visible during off-mesh connection.
                    if (m_nsmoothPath & 1)
                    {
                        dtVcopy(&m_smoothPath[m_nsmoothPath * 3], startPos);
                        m_nsmoothPath++;
                    }
                }
                // Move position at the other side of the off-mesh link.
                dtVcopy(iterPos, endPos);
                float eh = 0.0f;
                m_navQuery->getPolyHeight(polys[0], iterPos, &eh);
                iterPos[1] = eh;
            }
        }

        // Store results.
        if (m_nsmoothPath < MAX_SMOOTH)
        {
            dtVcopy(&m_smoothPath[m_nsmoothPath * 3], iterPos);
            m_nsmoothPath++;
        }
    }
    result.resize(m_nsmoothPath);
    for (auto i = 0; i < m_nsmoothPath; i++)
    {
        result[i] = Vector3d{ Vector3f{m_smoothPath[i * 3],m_smoothPath[i * 3 + 1],m_smoothPath[i * 3 + 2]} };
    }
    return result;
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
