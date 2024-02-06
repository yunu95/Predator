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
#include "fastlz.h"

namespace yunutyEngine
{
    struct FastLZCompressor : public dtTileCacheCompressor
    {
        virtual ~FastLZCompressor();

        virtual int maxCompressedSize(const int bufferSize)
        {
            return (int)(bufferSize * 1.05f);
        }

        virtual dtStatus compress(const unsigned char* buffer, const int bufferSize,
            unsigned char* compressed, const int /*maxCompressedSize*/, int* compressedSize)
        {
            *compressedSize = fastlz_compress((const void* const)buffer, bufferSize, compressed);
            return DT_SUCCESS;
        }

        virtual dtStatus decompress(const unsigned char* compressed, const int compressedSize,
            unsigned char* buffer, const int maxBufferSize, int* bufferSize)
        {
            *bufferSize = fastlz_decompress(compressed, compressedSize, buffer, maxBufferSize);
            return *bufferSize < 0 ? DT_FAILURE : DT_SUCCESS;
        }
    };

    FastLZCompressor::~FastLZCompressor()
    {
        // Defined out of line to fix the weak v-tables warning
    }

    struct LinearAllocator : public dtTileCacheAlloc
    {
        unsigned char* buffer;
        size_t capacity;
        size_t top;
        size_t high;

        LinearAllocator(const size_t cap) : buffer(0), capacity(0), top(0), high(0)
        {
            resize(cap);
        }

        virtual ~LinearAllocator();

        void resize(const size_t cap)
        {
            if (buffer) dtFree(buffer);
            buffer = (unsigned char*)dtAlloc(cap, DT_ALLOC_PERM);
            capacity = cap;
        }

        virtual void reset()
        {
            high = dtMax(high, top);
            top = 0;
        }

        virtual void* alloc(const size_t size)
        {
            if (!buffer)
                return 0;
            if (top + size > capacity)
                return 0;
            unsigned char* mem = &buffer[top];
            top += size;
            return mem;
        }

        virtual void free(void* /*ptr*/)
        {
            // Empty
        }
    };

    LinearAllocator::~LinearAllocator()
    {
        // Defined out of line to fix the weak v-tables warning
        dtFree(buffer);
    }

    struct MeshProcess : public dtTileCacheMeshProcess
    {
        //InputGeom* m_geom;

        inline MeshProcess() 
        {
        }

        virtual ~MeshProcess();

        //inline void init(InputGeom* geom)
        //{
        //    m_geom = geom;
        //}

        virtual void process(struct dtNavMeshCreateParams* params,
            unsigned char* polyAreas, unsigned short* polyFlags)
        {
            // Update poly flags from areas.
            for (int i = 0; i < params->polyCount; ++i)
            {
                if (polyAreas[i] == DT_TILECACHE_WALKABLE_AREA)
                {
                    polyAreas[i] = 0;
                    polyFlags[i] = 1;
                }
            }

            //// Pass in off-mesh connections.
            //if (m_geom)
            //{
            //    params->offMeshConVerts = m_geom->getOffMeshConnectionVerts();
            //    params->offMeshConRad = m_geom->getOffMeshConnectionRads();
            //    params->offMeshConDir = m_geom->getOffMeshConnectionDirs();
            //    params->offMeshConAreas = m_geom->getOffMeshConnectionAreas();
            //    params->offMeshConFlags = m_geom->getOffMeshConnectionFlags();
            //    params->offMeshConUserID = m_geom->getOffMeshConnectionId();
            //    params->offMeshConCount = m_geom->getOffMeshConnectionCount();
            //}
        }
    };

    MeshProcess::~MeshProcess()
    {
        // Defined out of line to fix the weak v-tables warning
    }

    static const int MAX_LAYERS = 32;

    struct TileCacheData
    {
        unsigned char* data;
        int dataSize;
    };

    struct RasterizationContext
    {
        RasterizationContext() :
            solid(0),
            triareas(0),
            lset(0),
            chf(0),
            ntiles(0)
        {
            memset(tiles, 0, sizeof(TileCacheData) * MAX_LAYERS);
        }

        ~RasterizationContext()
        {
            rcFreeHeightField(solid);
            delete[] triareas;
            rcFreeHeightfieldLayerSet(lset);
            rcFreeCompactHeightfield(chf);
            for (int i = 0; i < MAX_LAYERS; ++i)
            {
                dtFree(tiles[i].data);
                tiles[i].data = 0;
            }
        }

        rcHeightfield* solid;
        unsigned char* triareas;
        rcHeightfieldLayerSet* lset;
        rcCompactHeightfield* chf;
        TileCacheData tiles[MAX_LAYERS];
        int ntiles;
    };
}
