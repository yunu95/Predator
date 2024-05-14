#include "_YunutyNavigationFieldImpl.h"
#include <iostream>
#include <fstream>


namespace yunutyEngine
{
    bool NavigationField::Impl::handleBuild(const float* worldVertices, size_t verticesNum, const int* faces, size_t facesNum, const BuildSettings& buildSettings, std::ofstream* exportingFile)
    {
        static constexpr int EXPECTED_LAYERS_PER_TILE = 4;
        dtStatus status;

        // bmin, bmax 정보는 파일 입출력의 대상
        float bmin[3]{ std::numeric_limits<float>::max(),std::numeric_limits<float>::max(),std::numeric_limits<float>::max() };
        float bmax[3]{ -std::numeric_limits<float>::max(),-std::numeric_limits<float>::max(),-std::numeric_limits<float>::max() };
        // 바운더리 정보부터 설정
        for (auto i = 0; i < verticesNum; i++)
        {
            if (bmin[0] > worldVertices[i * 3])
                bmin[0] = worldVertices[i * 3];
            if (bmin[1] > worldVertices[i * 3 + 1])
                bmin[1] = worldVertices[i * 3 + 1];
            if (bmin[2] > worldVertices[i * 3 + 2])
                bmin[2] = worldVertices[i * 3 + 2];

            if (bmax[0] < worldVertices[i * 3])
                bmax[0] = worldVertices[i * 3];
            if (bmax[1] < worldVertices[i * 3 + 1])
                bmax[1] = worldVertices[i * 3 + 1];
            if (bmax[2] < worldVertices[i * 3 + 2])
                bmax[2] = worldVertices[i * 3 + 2];
        }

        // Init cache
        int gw = 0, gh = 0;
        rcCalcGridSize(bmin, bmax, buildSettings.divisionSizeXZ, &gw, &gh);
        const int ts = (int)buildSettings.tileSize;
        const int tw = (gw + ts - 1) / ts;
        const int th = (gh + ts - 1) / ts;

        // Generation params.
        rcConfig cfg;
        memset(&cfg, 0, sizeof(cfg));

        cfg.cs = buildSettings.divisionSizeXZ;
        cfg.ch = buildSettings.divisionSizeY;
        cfg.walkableSlopeAngle = buildSettings.walkableSlopeAngle;
        cfg.walkableHeight = (int)ceilf(2.0 / cfg.ch);
        cfg.walkableClimb = (int)floorf(0.9 / cfg.ch);
        cfg.walkableRadius = (int)ceilf(buildSettings.agentRadius / cfg.cs);
        cfg.maxEdgeLen = (int)(12.0 / buildSettings.divisionSizeXZ);
        cfg.maxSimplificationError = buildSettings.edgeMaxError;
        cfg.minRegionArea = (int)rcSqr(8);		// Note: area = size*size
        cfg.mergeRegionArea = (int)rcSqr(20);	// Note: area = size*size
        cfg.maxVertsPerPoly = (int)6;
        cfg.tileSize = (int)buildSettings.tileSize;
        cfg.borderSize = cfg.walkableRadius + 3; // Reserve enough padding.
        cfg.width = cfg.tileSize + cfg.borderSize * 2;
        cfg.height = cfg.tileSize + cfg.borderSize * 2;
        cfg.detailSampleDist = 6.0 < 0.9f ? 0 : buildSettings.divisionSizeXZ * 6.0;
        cfg.detailSampleMaxError = cfg.ch * 1.0;
        rcVcopy(cfg.bmin, bmin);
        rcVcopy(cfg.bmax, bmax);

        // Tile cache params.
        dtTileCacheParams tcparams;
        memset(&tcparams, 0, sizeof(tcparams));
        rcVcopy(tcparams.orig, bmin);
        tcparams.cs = buildSettings.divisionSizeXZ;
        tcparams.ch = buildSettings.divisionSizeY;
        tcparams.width = (int)buildSettings.tileSize;
        tcparams.height = (int)buildSettings.tileSize;
        tcparams.walkableHeight = buildSettings.walkableHeight;
        tcparams.walkableRadius = buildSettings.agentRadius;
        tcparams.walkableClimb = buildSettings.walkableClimb;
        tcparams.maxSimplificationError = buildSettings.edgeMaxError;
        tcparams.maxTiles = tw * th * EXPECTED_LAYERS_PER_TILE;
        tcparams.maxObstacles = 512;

        dtFreeTileCache(m_tileCache);

        m_tileCache = dtAllocTileCache();
        if (!m_tileCache)
        {
            m_ctx->log(RC_LOG_ERROR, "buildTiledNavigation: Could not allocate tile cache.");
            return false;
        }
        status = m_tileCache->init(&tcparams, m_talloc, m_tcomp, m_tmproc);
        if (dtStatusFailed(status))
        {
            m_ctx->log(RC_LOG_ERROR, "buildTiledNavigation: Could not init tile cache.");
            return false;
        }

        dtFreeNavMesh(m_navMesh);

        m_navMesh = dtAllocNavMesh();
        if (!m_navMesh)
        {
            m_ctx->log(RC_LOG_ERROR, "buildTiledNavigation: Could not allocate navmesh.");
            return false;
        }

        dtNavMeshParams params;
        memset(&params, 0, sizeof(params));
        rcVcopy(params.orig, bmin);

        int tileBits = rcMin((int)dtIlog2(dtNextPow2(tw * th * EXPECTED_LAYERS_PER_TILE)), 14);
        if (tileBits > 14) tileBits = 14;
        int polyBits = 22 - tileBits;
        m_maxTiles = 1 << tileBits;
        m_maxPolysPerTile = 1 << polyBits;

        params.tileWidth = buildSettings.tileSize * buildSettings.divisionSizeXZ;
        params.tileHeight = buildSettings.tileSize * buildSettings.divisionSizeXZ;
        params.maxTiles = m_maxTiles;
        params.maxPolys = m_maxPolysPerTile;

        status = m_navMesh->init(&params);
        if (dtStatusFailed(status))
        {
            m_ctx->log(RC_LOG_ERROR, "buildTiledNavigation: Could not init navmesh.");
            return false;
        }

        status = m_navQuery->init(m_navMesh, 2048);
        if (dtStatusFailed(status))
        {
            m_ctx->log(RC_LOG_ERROR, "buildTiledNavigation: Could not init Detour navmesh query");
            return false;
        }


        // Preprocess tiles.

        m_ctx->resetTimers();

        m_cacheLayerCount = 0;
        m_cacheCompressedSize = 0;
        m_cacheRawSize = 0;

        for (int y = 0; y < th; ++y)
        {
            for (int x = 0; x < tw; ++x)
            {
                // tileCacheData는 파일 저장의 대상
                TileCacheData tiles[MAX_LAYERS];
                memset(tiles, 0, sizeof(tiles));
                int ntiles = rasterizeTileLayers(worldVertices, verticesNum, faces, facesNum, x, y, cfg, tiles, MAX_LAYERS);

                for (int i = 0; i < ntiles; ++i)
                {
                    TileCacheData* tile = &tiles[i];
                    status = m_tileCache->addTile(tile->data, tile->dataSize, DT_COMPRESSEDTILE_FREE_DATA, 0);
                    if (dtStatusFailed(status))
                    {
                        dtFree(tile->data);
                        tile->data = 0;
                        continue;
                    }

                    m_cacheLayerCount++;
                    m_cacheCompressedSize += tile->dataSize;
                    m_cacheRawSize += calcLayerBufferSize(tcparams.width, tcparams.height);
                }
            }
        }

        // Build initial meshes
        m_ctx->startTimer(RC_TIMER_TOTAL);
        for (int y = 0; y < th; ++y)
            for (int x = 0; x < tw; ++x)
                m_tileCache->buildNavMeshTilesAt(x, y, m_navMesh);
        m_ctx->stopTimer(RC_TIMER_TOTAL);

        m_cacheBuildTimeMs = m_ctx->getAccumulatedTime(RC_TIMER_TOTAL) / 1000.0f;
        m_cacheBuildMemUsage = static_cast<unsigned int>(m_talloc->high);


        const dtNavMesh* nav = m_navMesh;
        int navmeshMemUsage = 0;
        for (int i = 0; i < nav->getMaxTiles(); ++i)
        {
            const dtMeshTile* tile = nav->getTile(i);
            if (tile->header)
                navmeshMemUsage += tile->dataSize;
        }
        printf("navmeshMemUsage = %.1f kB", navmeshMemUsage / 1024.0f);

        m_crowd->init(buildSettings.maxCrowdNumber, buildSettings.agentRadius, m_navMesh);
        return true;
    }
    bool NavigationField::Impl::handleBuild(std::ifstream* importingFile)
    {
        static constexpr int EXPECTED_LAYERS_PER_TILE = 4;
        dtStatus status;

        // 파일의 헤더를 읽어 빌드세팅 정보를 들여온다.
        FileHeader fileHeader;

        const int& ts = fileHeader.ts;
        const int& tw = fileHeader.tw;
        const int& th = fileHeader.th;
        const float* bmin = fileHeader.bmin;
        const float* bmax = fileHeader.bmax;
        const dtTileCacheParams& tcparams = fileHeader.tcparams;
        const dtNavMeshParams& params = fileHeader.navMeshParams;
        const BuildSettings& buildSettings = fileHeader.buildSettings;

        dtFreeTileCache(m_tileCache);

        m_tileCache = dtAllocTileCache();
        if (!m_tileCache)
        {
            m_ctx->log(RC_LOG_ERROR, "buildTiledNavigation: Could not allocate tile cache.");
            return false;
        }
        status = m_tileCache->init(&tcparams, m_talloc, m_tcomp, m_tmproc);
        if (dtStatusFailed(status))
        {
            m_ctx->log(RC_LOG_ERROR, "buildTiledNavigation: Could not init tile cache.");
            return false;
        }

        dtFreeNavMesh(m_navMesh);

        m_navMesh = dtAllocNavMesh();
        if (!m_navMesh)
        {
            m_ctx->log(RC_LOG_ERROR, "buildTiledNavigation: Could not allocate navmesh.");
            return false;
        }

        status = m_navMesh->init(&params);
        if (dtStatusFailed(status))
        {
            m_ctx->log(RC_LOG_ERROR, "buildTiledNavigation: Could not init navmesh.");
            return false;
        }

        status = m_navQuery->init(m_navMesh, 2048);
        if (dtStatusFailed(status))
        {
            m_ctx->log(RC_LOG_ERROR, "buildTiledNavigation: Could not init Detour navmesh query");
            return false;
        }


        // Preprocess tiles.
        m_ctx->resetTimers();

        m_cacheLayerCount = 0;
        m_cacheCompressedSize = 0;
        m_cacheRawSize = 0;

        for (int y = 0; y < th; ++y)
        {
            for (int x = 0; x < tw; ++x)
            {
                // tileCacheData는 파일 저장의 대상
                TileCacheData tiles[MAX_LAYERS];
                memset(tiles, 0, sizeof(tiles));
                static constexpr int maxTileDataSize = 4096;
                int nTiles;

                importingFile->read(reinterpret_cast<char*>(&nTiles), sizeof(int));

                for (int i = 0; i < nTiles; ++i)
                {
                    int tileDataSize;
                    byte tileData[maxTileDataSize];
                    importingFile->read(reinterpret_cast<char*>(&tileDataSize), sizeof(int));
                    assert(tileDataSize < maxTileDataSize);
                    importingFile->read(reinterpret_cast<char*>(tileData), tileDataSize);

                    status = m_tileCache->addTile(tileData, tileDataSize, DT_COMPRESSEDTILE_FREE_DATA, 0);
                    if (dtStatusFailed(status))
                    {
                        continue;
                    }

                    m_cacheLayerCount++;
                    m_cacheCompressedSize += tileDataSize;
                    m_cacheRawSize += calcLayerBufferSize(tcparams.width, tcparams.height);
                }
            }
        }

        // Build initial meshes
        m_ctx->startTimer(RC_TIMER_TOTAL);
        for (int y = 0; y < th; ++y)
            for (int x = 0; x < tw; ++x)
                m_tileCache->buildNavMeshTilesAt(x, y, m_navMesh);
        m_ctx->stopTimer(RC_TIMER_TOTAL);

        m_cacheBuildTimeMs = m_ctx->getAccumulatedTime(RC_TIMER_TOTAL) / 1000.0f;
        m_cacheBuildMemUsage = static_cast<unsigned int>(m_talloc->high);


        const dtNavMesh* nav = m_navMesh;
        int navmeshMemUsage = 0;
        for (int i = 0; i < nav->getMaxTiles(); ++i)
        {
            const dtMeshTile* tile = nav->getTile(i);
            if (tile->header)
                navmeshMemUsage += tile->dataSize;
        }
        printf("navmeshMemUsage = %.1f kB", navmeshMemUsage / 1024.0f);

        m_crowd->init(buildSettings.maxCrowdNumber, buildSettings.agentRadius, m_navMesh);
        return true;
    }
    int NavigationField::Impl::rasterizeTileLayers(const float* worldVertices, size_t verticesNum, const int* faces, size_t facesNum, const int tx, const int ty, const rcConfig& cfg, struct TileCacheData* tiles, const int maxTiles)
    {
        FastLZCompressor comp;
        RasterizationContext rc;

        const float* verts = worldVertices;
        const int nverts = verticesNum;
        rcChunkyTriMesh chunkyMesh;
        rcCreateChunkyTriMesh(verts, faces, facesNum, 256, &chunkyMesh);

        // Tile bounds.
        const float tcs = cfg.tileSize * cfg.cs;

        rcConfig tcfg;
        memcpy(&tcfg, &cfg, sizeof(tcfg));

        tcfg.bmin[0] = cfg.bmin[0] + tx * tcs;
        tcfg.bmin[1] = cfg.bmin[1];
        tcfg.bmin[2] = cfg.bmin[2] + ty * tcs;
        tcfg.bmax[0] = cfg.bmin[0] + (tx + 1) * tcs;
        tcfg.bmax[1] = cfg.bmax[1];
        tcfg.bmax[2] = cfg.bmin[2] + (ty + 1) * tcs;
        tcfg.bmin[0] -= tcfg.borderSize * tcfg.cs;
        tcfg.bmin[2] -= tcfg.borderSize * tcfg.cs;
        tcfg.bmax[0] += tcfg.borderSize * tcfg.cs;
        tcfg.bmax[2] += tcfg.borderSize * tcfg.cs;

        // Allocate voxel heightfield where we rasterize our input data to.
        rc.solid = rcAllocHeightfield();
        if (!rc.solid)
        {
            m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'solid'.");
            return 0;
        }
        if (!rcCreateHeightfield(m_ctx, *rc.solid, tcfg.width, tcfg.height, tcfg.bmin, tcfg.bmax, tcfg.cs, tcfg.ch))
        {
            m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not create solid heightfield.");
            return 0;
        }

        // Allocate array that can hold triangle flags.
        // If you have multiple meshes you need to process, allocate
        // and array which can hold the max number of triangles you need to process.
        rc.triareas = new unsigned char[chunkyMesh.maxTrisPerChunk];
        if (!rc.triareas)
        {
            m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'm_triareas' (%d).", chunkyMesh.maxTrisPerChunk);
            return 0;
        }

        float tbmin[2], tbmax[2];
        tbmin[0] = tcfg.bmin[0];
        tbmin[1] = tcfg.bmin[2];
        tbmax[0] = tcfg.bmax[0];
        tbmax[1] = tcfg.bmax[2];
        int cid[512];// TODO: Make grow when returning too many items.
        const int ncid = rcGetChunksOverlappingRect(&chunkyMesh, tbmin, tbmax, cid, 512);
        if (!ncid)
        {
            return 0; // empty
        }

        for (int i = 0; i < ncid; ++i)
        {
            const rcChunkyTriMeshNode& node = chunkyMesh.nodes[cid[i]];
            const int* tris = &chunkyMesh.tris[node.i * 3];
            const int ntris = node.n;

            memset(rc.triareas, 0, ntris * sizeof(unsigned char));
            rcMarkWalkableTriangles(m_ctx, tcfg.walkableSlopeAngle,
                verts, nverts, tris, ntris, rc.triareas);

            if (!rcRasterizeTriangles(m_ctx, verts, nverts, tris, rc.triareas, ntris, *rc.solid, tcfg.walkableClimb))
                return 0;
        }

        // Once all geometry is rasterized, we do initial pass of filtering to
        // remove unwanted overhangs caused by the conservative rasterization
        // as well as filter spans where the character cannot possibly stand.
        if (m_filterLowHangingObstacles)
            rcFilterLowHangingWalkableObstacles(m_ctx, tcfg.walkableClimb, *rc.solid);
        if (m_filterLedgeSpans)
            rcFilterLedgeSpans(m_ctx, tcfg.walkableHeight, tcfg.walkableClimb, *rc.solid);
        if (m_filterWalkableLowHeightSpans)
            rcFilterWalkableLowHeightSpans(m_ctx, tcfg.walkableHeight, *rc.solid);


        rc.chf = rcAllocCompactHeightfield();
        if (!rc.chf)
        {
            m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'chf'.");
            return 0;
        }
        if (!rcBuildCompactHeightfield(m_ctx, tcfg.walkableHeight, tcfg.walkableClimb, *rc.solid, *rc.chf))
        {
            m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build compact data.");
            return 0;
        }

        // Erode the walkable area by agent radius.
        if (!rcErodeWalkableArea(m_ctx, tcfg.walkableRadius, *rc.chf))
        {
            m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not erode.");
            return 0;
        }

        rc.lset = rcAllocHeightfieldLayerSet();
        if (!rc.lset)
        {
            m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'lset'.");
            return 0;
        }
        if (!rcBuildHeightfieldLayers(m_ctx, *rc.chf, tcfg.borderSize, tcfg.walkableHeight, *rc.lset))
        {
            m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build heighfield layers.");
            return 0;
        }

        rc.ntiles = 0;
        for (int i = 0; i < rcMin(rc.lset->nlayers, MAX_LAYERS); ++i)
        {
            TileCacheData* tile = &rc.tiles[rc.ntiles++];
            const rcHeightfieldLayer* layer = &rc.lset->layers[i];

            // Store header
            dtTileCacheLayerHeader header;
            header.magic = DT_TILECACHE_MAGIC;
            header.version = DT_TILECACHE_VERSION;

            // Tile layer location in the navmesh.
            header.tx = tx;
            header.ty = ty;
            header.tlayer = i;
            dtVcopy(header.bmin, layer->bmin);
            dtVcopy(header.bmax, layer->bmax);

            // Tile info.
            header.width = (unsigned char)layer->width;
            header.height = (unsigned char)layer->height;
            header.minx = (unsigned char)layer->minx;
            header.maxx = (unsigned char)layer->maxx;
            header.miny = (unsigned char)layer->miny;
            header.maxy = (unsigned char)layer->maxy;
            header.hmin = (unsigned short)layer->hmin;
            header.hmax = (unsigned short)layer->hmax;

            dtStatus status = dtBuildTileCacheLayer(&comp, &header, layer->heights, layer->areas, layer->cons,
                &tile->data, &tile->dataSize);
            if (dtStatusFailed(status))
            {
                return 0;
            }
        }

        // Transfer ownsership of tile data from build context to the caller.
        int n = 0;
        for (int i = 0; i < rcMin(rc.ntiles, maxTiles); ++i)
        {
            tiles[n++] = rc.tiles[i];
            rc.tiles[i].data = 0;
            rc.tiles[i].dataSize = 0;
        }

        return n;
    }
    int NavigationField::Impl::calcLayerBufferSize(const int gridWidth, const int gridHeight)
    {
        const int headerSize = dtAlign4(sizeof(dtTileCacheLayerHeader));
        const int gridSize = gridWidth * gridHeight;
        return headerSize + gridSize * 4;
    }
    dtObstacleRef NavigationField::Impl::AddBoxObstacle(Vector3f center, Vector3f halfExtents, const float yRadians)
    {
        if (!m_tileCache)
            return 0;
        dtObstacleRef result;
        m_tileCache->addBoxObstacle(reinterpret_cast<float*>(&center), reinterpret_cast<float*>(&halfExtents), yRadians, &result);
        return result;
    }
    dtObstacleRef NavigationField::Impl::AddCylinderObstacle(Vector3f center, float radius, const float height)
    {
        if (!m_tileCache)
            return 0;
        dtObstacleRef result;
        m_tileCache->addObstacle(reinterpret_cast<float*>(&center), radius, height, &result);
        return result;
    }
    void NavigationField::Impl::DeleteObstacle(dtObstacleRef obstacleRef)
    {
        if (!m_tileCache)
            return;
        m_tileCache->removeObstacle(obstacleRef);
    }
}
