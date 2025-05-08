#pragma once
#include <Box2D/box2d.h>
#include <Box2D/src/world.h>
#include <glm/glm.hpp>


namespace Engine
{


    class Rigidbody2D
    {
    public:
        enum class BodyType { Static = 0, Dynamic, Kinematic };

        Rigidbody2D() = default;
        Rigidbody2D(BodyType type) : m_Type(type) {}
        ~Rigidbody2D() { DestroyBody(); }

        void Update() {};

        void DestroyBody()
        {
            if (m_Body)
            {
                //m_Body->GetWorld()->DestroyBody(m_Body);
                m_Body = nullptr;
            }
        }

        void SetType(BodyType type)
        {
            m_Type = type;
            //if (m_Body)
                //m_Body->SetType((b2BodyType)type);
        }
        b2Body* GetBody() const { return m_Body; }

    private:
        BodyType m_Type = BodyType::Static;
        b2Body* m_Body = nullptr;
    };


}