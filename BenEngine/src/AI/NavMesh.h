#pragma once
#include <iostream>
#include <vector>
#include "Recast.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshQuery.h"
#include "RecastAlloc.h"
#include "DetourCommon.h"

namespace Engine
{

    class NavMesh
    {
    public:
        NavMesh(){}

        ~NavMesh() {
            //if (m_navMesh) {
            //    m_navMesh->removeAllTiles();
            //    delete m_navMesh;
            //}
            //if (m_navQuery) {
            //    delete m_navQuery;
            //}
        }

        void Generate();

        void BuildSimpleGeometry(rcCompactHeightfield& solid);
        dtNavMesh* BuildNavMesh(const rcCompactHeightfield& solid);

        dtNavMesh* GetNavMesh() { return m_navMesh; }

    protected:

        const float NAVMESH_TILE_SIZE = 3.0f; // Size of each tile in the NavMesh
        const float AGENT_RADIUS = 0.6f;       // Radius of the agent (used for pathfinding)
        const float AGENT_HEIGHT = 2.0f;       // Height of the agent
        const float AGENT_MAX_SLOPE = 90.0f;   // Maximum slope the agent can navigate
        const float AGENT_MAX_CLIMB = INFINITE;    // Maximum climb height for the agent

        int width = 10, height = 10;
        std::vector<std::vector<int>> tiles;
        dtNavMesh* m_navMesh = nullptr;



    };

}