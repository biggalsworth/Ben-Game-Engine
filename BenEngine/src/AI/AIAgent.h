#include <vector>
#include <box2d/box2d.h>
#include "DetourCommon.h"
#include "DetourNavMeshQuery.h"

namespace Engine
{
    class AIAgent
    {
    public:
        AIAgent(b2BodyId* body, dtNavMesh* navMesh)
        {
            m_Body = body;
            m_NavMesh = navMesh;
        }

        void SetTarget(b2Vec2& target);

        void Update(float deltaTime)
        {
            FollowWaypoints(deltaTime); // Move along the computed path
        }

        void CalculatePath();

        void FollowWaypoints(float deltaTime);

    protected:
        b2BodyId* m_Body;
        dtNavMesh* m_NavMesh;
        b2Vec2 m_Target;
        std::vector<b2Vec2> m_Waypoints;
    };
}