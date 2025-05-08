#include "Engine_PCH.h"
#include "Raycast.h"
#include "PhysicsSystem.h"

namespace Engine
{
    b2CastResultFcn* RayCast2D::fcns[] = { RayCastAnyCallback, RayCastClosestCallback, RayCastMultipleCallback,
                                        RayCastSortedCallback };

    float RayCast2D::RayCastClosestCallback(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void* context)
    {
        RayCastContext* rayContext = (RayCastContext*)context;

        PhysicsSystem::ShapeData* userData = (PhysicsSystem::ShapeData*)b2Shape_GetUserData(shapeId);
        if (userData != nullptr && userData->ignore)
        {
            // By returning -1, we instruct the calling code to ignore this shape and
            // continue the ray-cast to the next shape.
            return -1.0f;
        }

        rayContext->points[0] = point;
        rayContext->normals[0] = normal;
        rayContext->fractions[0] = fraction;
        rayContext->count = 1;
        rayContext->entity[0] = userData->entity;

        // By returning the current fraction, we instruct the calling code to clip the ray and
        // continue the ray-cast to the next shape. WARNING: do not assume that shapes
        // are reported in order. However, by clipping, we can always get the closest shape.
        return fraction;
    }

    float RayCast2D::RayCastAnyCallback(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void* context)

    {
        RayCastContext* rayContext = (RayCastContext*)context;

        PhysicsSystem::ShapeData* userData = (PhysicsSystem::ShapeData*)b2Shape_GetUserData(shapeId);

        uint32_t* A = reinterpret_cast<uint32_t*>((intptr_t)b2Shape_GetUserData(shapeId));

        if (userData != nullptr && userData->ignore)
        {
            // By returning -1, we instruct the calling code to ignore this shape and
            // continue the ray-cast to the next shape.
            return -1.0f;
        }

        if (userData == nullptr)
        {
            LOG_WARN("Warning: Shape userData is nullptr!");
        }


        rayContext->points[0] = point;
        rayContext->normals[0] = normal;
        rayContext->fractions[0] = fraction;
        rayContext->count = 1;
        rayContext->entity[0] = userData->entity;

        // At this point we have a hit, so we know the ray is obstructed.
        // By returning 0, we instruct the calling code to terminate the ray-cast.
        return 0.0f;
    }

    float RayCast2D::RayCastMultipleCallback(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void* context)
    {
        RayCastContext* rayContext = (RayCastContext*)context;

        PhysicsSystem::ShapeData* userData = (PhysicsSystem::ShapeData*)b2Shape_GetUserData(shapeId);

        if (userData != nullptr && userData->ignore)
        {
            // By returning -1, we instruct the calling code to ignore this shape and
            // continue the ray-cast to the next shape.
            return -1.0f;
        }

        int count = rayContext->count;
        assert(count < 3);

        rayContext->points[count] = point;
        rayContext->normals[count] = normal;
        rayContext->fractions[count] = fraction;
        rayContext->count = count + 1;
        rayContext->entity[count] = userData->entity;

        if (rayContext->count == 3)
        {
            // At this point the buffer is full.
            // By returning 0, we instruct the calling code to terminate the ray-cast.
            return 0.0f;
        }

        // By returning 1, we instruct the caller to continue without clipping the ray.
        return 1.0f;
    }


    float RayCast2D::RayCastSortedCallback(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void* context)
    {
        RayCastContext* rayContext = (RayCastContext*)context;

        PhysicsSystem::ShapeData* userData = (PhysicsSystem::ShapeData*)b2Shape_GetUserData(shapeId);
        if (userData != nullptr && userData->ignore)
        {
            // By returning -1, we instruct the calling code to ignore this shape and
            // continue the ray-cast to the next shape.
            return -1.0f;
        }

        int count = rayContext->count;

        assert(count <= 3);

        int index = 3;
        while (fraction < rayContext->fractions[index - 1])
        {
            index -= 1;

            if (index == 0)
            {
                break;
            }
        }

        if (index == 3)
        {
            // not closer, continue but tell the caller not to consider fractions further than the largest fraction acquired
            // this only happens once the buffer is full
            assert(rayContext->count == 3);
            assert(rayContext->fractions[2] <= 1.0f);
            return rayContext->fractions[2];
        }

        for (int j = 2; j > index; --j)
        {
            rayContext->points[j] = rayContext->points[j - 1];
            rayContext->normals[j] = rayContext->normals[j - 1];
            rayContext->fractions[j] = rayContext->fractions[j - 1];
        }

        rayContext->points[index] = point;
        rayContext->normals[index] = normal;
        rayContext->fractions[index] = fraction;
        rayContext->count = count < 3 ? count + 1 : 3;
        rayContext->entity[index] = userData->entity;

        if (rayContext->count == 3)
        {
            return rayContext->fractions[2];
        }

        // By returning 1, we instruct the caller to continue without clipping the ray.
        return 1.0f;
    }

    std::vector<uint32_t> RayCast2D::overlapResults;

    bool RayCast2D::CircleOverlap(b2ShapeId shapeId, void* context)
    {

        PhysicsSystem::ShapeData* userData = (PhysicsSystem::ShapeData*)b2Shape_GetUserData(shapeId);

        if (userData != nullptr && userData->ignore)
        {
            // By returning -1, we instruct the calling code to ignore this shape and
            // continue the ray-cast to the next shape.
            return -1.0f;
        }

        if (userData == nullptr)
        {
            LOG_WARN("Warning: Shape userData is nullptr!");
        }


        overlapResults.push_back( userData->entity );

        return true; // Continue checking other shapes
    }
    


    //void Raycast::CastRay(GLFWwindow* Context)
	//{
	//	//at global scope
	//	float currentRayAngle = 0;
    //
	//	//in Step() function
	//	//currentRayAngle += 360 / 20.0 / 60.0 * DEGTORAD; //one revolution every 20 seconds
    //
	//	//calculate points of ray
	//	float rayLength = 25; //long enough to hit the walls
	//	b2Vec2 p1 = { 0.0f, 20.0f }; //center of scene
	//	b2Vec2 p2 = p1 + rayLength * b2Vec2{ sinf(currentRayAngle) , cosf(currentRayAngle) };
    //
    //
    //    // Compile Vertex Shader
    //    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    //    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    //    glCompileShader(vertexShader);
    //
    //    // Compile Fragment Shader
    //    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    //    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    //    glCompileShader(fragmentShader);
    //
    //    // Link Shader Program
    //    GLuint shaderProgram = glCreateProgram();
    //    glAttachShader(shaderProgram, vertexShader);
    //    glAttachShader(shaderProgram, fragmentShader);
    //    glLinkProgram(shaderProgram);
    //
    //    glDeleteShader(vertexShader);
    //    glDeleteShader(fragmentShader);
    //
    //    // Convert to OpenGL screen space (-1 to 1)
    //    float normalizedP1[3] = { p1.x / 50.0f, p1.y / 50.0f, 0.0f };
    //    float normalizedP2[3] = { p2.x / 50.0f, p2.y / 50.0f, 0.0f };
    //
    //    float lineVertices[] = {
    //        0.0, 0.0, 0.0,
    //        1.0, 0.0, 0.0
    //    };
    //
    //    // Generate VBO and VAO
    //    GLuint VBO, VAO;
    //    glGenVertexArrays(1, &VAO);
    //    glGenBuffers(1, &VBO);
    //
    //    // Bind and set data
    //    glBindVertexArray(VAO);
    //    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //    glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_STATIC_DRAW);
    //
    //    // Link vertex data to shader
    //    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    //    glEnableVertexAttribArray(0);
    //
    //    glUseProgram(shaderProgram);
    //    glBindVertexArray(VAO);
    //    glDrawArrays(GL_LINES, 0, 2); // Draw the line
    //
	//}
}