#include "Engine_PCH.h"
#include "AIAgent.h"
#include "box2d/box2d.h"
#include <Box2D/src/body.h>

namespace Engine
{

    void AIAgent::SetTarget(b2Vec2& target)
    {
        m_Target = target;
        CalculatePath(); // Find path on the navmesh
    }

    void AIAgent::CalculatePath()
    {
        dtNavMeshQuery navQuery;
        navQuery.init(m_NavMesh, 2048);

        dtQueryFilter filter;
        filter.setIncludeFlags(0xFFFF); // Accept all walkable areas

        dtPolyRef startRef, endRef;
        float startPos[] = { b2Body_GetPosition(*m_Body).x, 0, b2Body_GetPosition(*m_Body).y };
        float endPos[] = { m_Target.x, 0, m_Target.y };
        float extents[] = { 5.0f, 5.0f, 5.0f };

        LOG_WARN("START POS ", startPos[0], " : ", startPos[2]);
        LOG_WARN("END POS ", endPos[0], " : ", endPos[2]);

        navQuery.findNearestPoly(startPos, extents, &filter, &startRef, nullptr);
        navQuery.findNearestPoly(endPos, extents, &filter, &endRef, nullptr);

        unsigned int path[256];
        int pathCount;
        navQuery.findPath(startRef, endRef, startPos, endPos, &filter, path, &pathCount, int(5));

        m_Waypoints.clear();
        for (int i = 0; i < pathCount; i++)
        {
            b2Vec2 waypoint;
            waypoint.x = path[i * 3];
            waypoint.y = path[i * 3 + 1];
            m_Waypoints.push_back(waypoint);
            //m_Waypoints.emplace_back(path[i * 3], path[i * 3 + 1]); // X & Y, ignore Z
        }
    }

    void AIAgent::FollowWaypoints(float deltaTime)
    {
        if (!m_Waypoints.empty())
        {
            b2Vec2 target = m_Waypoints.front();
            b2Vec2 currentPos = b2Body_GetPosition(*m_Body);
            b2Vec2 direction = target - currentPos;

            if (b2LengthSquared(direction) > 0.1f) // If not at target
            {
                b2Normalize(direction);
                b2Body_SetLinearVelocity(*m_Body, direction * 5.0f); // Set movement speed
            }
            else
            {
                m_Waypoints.erase(m_Waypoints.begin()); // Move to next waypoint
            }
        }
    }
}