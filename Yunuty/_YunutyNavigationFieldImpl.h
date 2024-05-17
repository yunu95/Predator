#pragma once
#include "YunutyNavigationField.h"
#include "Recast.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshBuilder.h"
#include "DetourNavMeshQuery.h"
#include "DetourTileCache.h"
#include "DetourTileCacheBuilder.h"
#include "DetourCommon.h"
#include "DetourMath.h"
#include "DetourAlloc.h"
#include "DetourAssert.h"
#include "DetourCrowd.h"
#include "_YunutyCustomDtClasses.h"
#include "ChunkyTriMesh.h"

namespace yunutyEngine
{
    class NavigationField::Impl
    {
    private:
        // 파일 입출력시 파일의 첫 부분에 저장되는 헤더 정보
        struct FileHeader
        {
            BuildSettings buildSettings;
            rcConfig cfg;
            dtTileCacheParams tcparams;
            dtNavMeshParams navMeshParams;
            float bmin[3];
            float bmax[3];
            int ts;
            int tw;
            int th;
            int gw;
            int gh;
        };
        /*struct TileHeader
        {
            int nTiles;
        };*/
        Impl(NavigationField* navFieldComponent) :navFieldComponent(navFieldComponent)
        {
            m_navQuery = dtAllocNavMeshQuery();
            m_crowd = dtAllocCrowd();
            m_ctx = new rcContext;
            m_talloc = new LinearAllocator(32000);
            m_tcomp = new FastLZCompressor;
            m_tmproc = new MeshProcess;
        }
        virtual ~Impl()
        {
            dtFreeCrowd(m_crowd);
            m_crowd = nullptr;
            dtFreeNavMeshQuery(m_navQuery);
            m_navQuery = nullptr;
            delete m_ctx;
            if (m_navMesh)
            {
                dtFreeNavMesh(m_navMesh);
                m_navMesh = nullptr;
            }
            if (m_tileCache)
            {
                dtFreeTileCache(m_tileCache);
                m_tileCache = nullptr;
            }
            delete m_talloc;
            delete m_tcomp;
            delete m_tmproc;
        }
        friend NavigationField;
    public:
        bool handleBuild(std::ifstream* importingFile = nullptr);
        bool handleBuild(const float* worldVertices, size_t verticesNum, const int* faces, size_t facesNum, const BuildSettings& buildSettings, std::ofstream* exportingFile = nullptr);
        int rasterizeTileLayers(const float* worldVertices, size_t verticesNum, const int* faces, size_t facesNum, const int tx, const int ty, const rcConfig& cfg, struct TileCacheData* tiles, const int maxTiles);
        int calcLayerBufferSize(const int gridWidth, const int gridHeight);
        dtObstacleRef AddBoxObstacle(Vector3f center, Vector3f halfExtents, const float yRadians);
        dtObstacleRef AddCylinderObstacle(Vector3f center, float radius, const float height);
        void DeleteObstacle(dtObstacleRef obstacleRef);

        NavigationField* navFieldComponent;

        //class InputGeom* m_geom;
        class dtNavMesh* m_navMesh;
        class dtNavMeshQuery* m_navQuery;
        class dtCrowd* m_crowd;

        unsigned char m_navMeshDrawFlags;

        bool m_filterLowHangingObstacles;
        bool m_filterLedgeSpans;
        bool m_filterWalkableLowHeightSpans;
        rcContext* m_ctx;

        struct LinearAllocator* m_talloc;
        struct FastLZCompressor* m_tcomp;
        struct MeshProcess* m_tmproc;

        class dtTileCache* m_tileCache;
        dtPolyRef lastRef = 0;

        float m_cacheBuildTimeMs;
        int m_cacheCompressedSize;
        int m_cacheRawSize;
        int m_cacheLayerCount;
        unsigned int m_cacheBuildMemUsage;
        int m_maxTiles;
        int m_maxPolysPerTile;
        float m_tileSize;
    };
}
