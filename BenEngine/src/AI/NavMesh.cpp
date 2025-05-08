#include "Engine_PCH.h"
#include "NavMesh.h"
#include <Recast/Include/DetourNavMeshBuilder.h>


namespace Engine
{
    void NavMesh::Generate()
    {
        dtFreeNavMesh(m_navMesh);
        m_navMesh = dtAllocNavMesh();
        rcCompactHeightfield solid;
        BuildSimpleGeometry(solid);

        //Build NavMesh from the geometry
        m_navMesh = BuildNavMesh(solid);
    }

    // Example Geometry: A simple grid-based world with some obstacles
    void NavMesh::BuildSimpleGeometry(rcCompactHeightfield& solid)
    {

        // fill it with an empty grid (flat terrain).

        // Example: A 100x100 grid with some random obstacles
        int width = 100;
        int height = 100;
        rcConfig cfg;
        cfg.cs = 0.5f;  // Cell size
        cfg.ch = 0.5f;  // Cell height
        cfg.walkableSlopeAngle = AGENT_MAX_SLOPE;
        cfg.walkableHeight = AGENT_HEIGHT;

        solid.width = width;
        solid.height = height;
        solid.cs = cfg.cs;
        solid.ch = cfg.ch;
        solid.walkableClimb = cfg.walkableSlopeAngle;
        solid.walkableHeight = cfg.walkableHeight;

        solid.bmin[0] = 0.0f;  // Min X
        solid.bmin[1] = 0.0f;  // Min Y
        solid.bmin[2] = 0.0f;  // Min Z

        solid.bmax[0] = width * solid.cs;  // Max X (width * cell size)
        solid.bmax[1] = height * solid.ch; // Max Y (height * cell height)
        solid.bmax[2] = width * solid.cs;  // Max Z (assuming a square grid)

    }

    dtNavMesh* NavMesh::BuildNavMesh(const rcCompactHeightfield& solid)
    {
        //rcConfig cfg;
        //cfg.cs = 0.3f;  // Cell size (smaller for more detail)
        //cfg.ch = 0.2f;  // Cell height
        //cfg.walkableRadius = AGENT_RADIUS;
        //cfg.walkableHeight = AGENT_HEIGHT;
        //cfg.walkableSlopeAngle = AGENT_MAX_SLOPE;
        //cfg.walkableClimb = AGENT_MAX_CLIMB;
        //rcContext ctx;
        //
        //rcPolyMesh* mesh = new rcPolyMesh();
        //// Your code to populate your geometry into a mesh using Recast
        //
        //rcContourSet* contourSet = new rcContourSet();
        //rcBuildContours(&ctx, solid, cfg.walkableSlopeAngle, 2, *contourSet);
        //
        //rcBuildPolyMesh(&ctx, *contourSet, 6.0f, *mesh);


        dtNavMeshParams params;
        params.orig[0] = 0.0f;
        params.orig[1] = 0.0f;
        params.orig[2] = 0.0f;  // We don't care about Z in 2D, but it's still needed for the API
        params.tileWidth = NAVMESH_TILE_SIZE;
        params.tileHeight = NAVMESH_TILE_SIZE;
        params.maxTiles = 128;
        params.maxPolys = 4096;
        dtNavMesh* navMesh = dtAllocNavMesh();
        if (!navMesh)
        {
            return nullptr; // Failed to allocate the navmesh
        }
        const int maxTiles = 128;
        const int maxPolys = 4096;
        if (navMesh->init(&params) != DT_SUCCESS)
        {
            return nullptr; // Failed to create tile data
        }

        dtStatus status;
        dtNavMeshQuery* query = new dtNavMeshQuery();
        status = query->init(m_navMesh, 2048);
        if (dtStatusFailed(status))
        {
            LOG_ERROR("buildTiledNavigation: Could not init Detour navmesh query");
            return nullptr;
        }


        //to build the tiles, not yet working

        //rcContext ctx;
        //// Create contour set
        //rcContourSet* contourSet = new rcContourSet();
        //rcBuildContours(solid, 2.0f, *contourSet);  // Build contours with a max edge length of 2.0f
        //
        //// Create poly mesh
        //rcPolyMesh* polyMesh = new rcPolyMesh();
        //rcBuildPolyMesh(&ctx, *contourSet, 10, *polyMesh);  // Build polygons from contours
        //
        //// Divide poly mesh into tiles
        //int tileXCount = (int)std::ceil((solid.bmax[0] - solid.bmin[0]) / NAVMESH_TILE_SIZE);
        //int tileYCount = (int)std::ceil((solid.bmax[2] - solid.bmin[2]) / NAVMESH_TILE_SIZE);
        //
        //// Add tiles to navmesh
        //for (int x = 0; x < tileXCount; ++x)
        //{
        //    for (int y = 0; y < tileYCount; ++y)
        //    {
        //        // Create a submesh for each tile
        //        int minX = x * NAVMESH_TILE_SIZE;
        //        int minY = y * NAVMESH_TILE_SIZE;
        //        int maxX = (x + 1) * NAVMESH_TILE_SIZE;
        //        int maxY = (y + 1) * NAVMESH_TILE_SIZE;
        //
        //        // Create the tile and add it to the navmesh
        //        dtTileRef tileRef = 0;
        //        navMesh->addTile(&polyMesh->verts, polyMesh->getVertCount(), polyMesh->getPolys(), polyMesh->getPolyCount(), DT_TILE_FREE_DATA, &tileRef);
        //    }
        //}

        return navMesh;

    }
}
