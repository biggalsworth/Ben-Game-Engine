#include "Engine_PCH.h"
#include "Renderer2D.h"

#include "Renderer/Data/VertexArray.h"
#include "Renderer/Shader/Shader.h"
#include "Renderer/Data/UniformBuffer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Renderer2DData.h"

#include <glad/glad.h>

namespace Engine
{
	static Renderer2DData s_Data;

    void Renderer2D::Init()
    {
        s_Data.QuadVertexArray = VertexArray::Create();

        s_Data.QuadVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));
        s_Data.QuadVertexBuffer->SetLayout({
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float4, "a_Color" },
            { ShaderDataType::Float2, "a_TexCoord" },
            { ShaderDataType::Float2,  "a_TilingFactor" },
            //{ ShaderDataType::Float,  "a_TilingFactor" }, //make stuff go crazy
            { ShaderDataType::Int,  "a_EntityID" }
            //{ ShaderDataType::Int, "a_TexIndex" }
            });


        s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

        //s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];
        s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];

        uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];
    
        uint32_t offset = 0;
        for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6)
        {
            quadIndices[i + 0] = offset + 0;
            quadIndices[i + 1] = offset + 1;
            quadIndices[i + 2] = offset + 2;

            quadIndices[i + 3] = offset + 2;
            quadIndices[i + 4] = offset + 3;
            quadIndices[i + 5] = offset + 0;

            offset += 4;
        }

        Ref<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, s_Data.MaxIndices);
        s_Data.QuadVertexArray->SetIndexBuffer(quadIB);
        delete[] quadIndices;


        //collider drawer
            s_Data.ColliderVertexArray = VertexArray::Create();
            s_Data.CircleColliderVertexArray = VertexArray::Create();

            s_Data.ColliderVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadVertex));
            s_Data.ColliderVertexBuffer->SetLayout(s_Data.QuadVertexBuffer->GetLayout());        

            s_Data.CircleColliderVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(CircleVertex));
            s_Data.CircleColliderVertexBuffer->SetLayout({
                {ShaderDataType::Float3,    "a_WorldPosition"   },
                {ShaderDataType::Float3,    "a_LocalPosition"   },
                {ShaderDataType::Float4,    "a_Color"           },
                {ShaderDataType::Float,     "a_Thickness"       },
                {ShaderDataType::Float,     "a_Fade"            },
                {ShaderDataType::Int,       "a_EntityID"        },
                {ShaderDataType::Float,     "a_Radius"          },
                });

            s_Data.ColliderVertexArray->AddVertexBuffer(s_Data.ColliderVertexBuffer);
            s_Data.ColliderVertexArray->SetIndexBuffer(quadIB);

            s_Data.CircleColliderVertexArray->AddVertexBuffer(s_Data.CircleColliderVertexBuffer);
            s_Data.CircleColliderVertexArray->SetIndexBuffer(quadIB);

            s_Data.ColliderVertexBufferBase = new QuadVertex[s_Data.MaxVertices];
            s_Data.CircleColliderVertexBufferBase = new CircleVertex[s_Data.MaxVertices];

        //Circles
        s_Data.CircleVertexArray = VertexArray::Create();

        s_Data.CircleVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(CircleVertex));
        s_Data.CircleVertexBuffer->SetLayout({
            {ShaderDataType::Float3,    "a_WorldPosition"   },
            {ShaderDataType::Float3,    "a_LocalPosition"   },
            {ShaderDataType::Float4,    "a_Color"           },
            {ShaderDataType::Float,     "a_Thickness"       },
            {ShaderDataType::Float,     "a_Fade"            },
            {ShaderDataType::Int,       "a_EntityID"        },
            {ShaderDataType::Float,     "a_Radius"          },
            });

        s_Data.CircleVertexArray->AddVertexBuffer(s_Data.CircleVertexBuffer);
        s_Data.CircleVertexArray->SetIndexBuffer(quadIB);
        s_Data.CircleVertexBufferBase = new CircleVertex[s_Data.MaxVertices];
        //End of circles
        


        #pragma region Lines
        s_Data.LineVertexArray = VertexArray::Create();

        s_Data.LineVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(LineVertex));
        s_Data.LineVertexBuffer->SetLayout({
            {ShaderDataType::Float3,    "a_Position"        },
            {ShaderDataType::Float4,    "a_Color"           },
            {ShaderDataType::Float,     "a_Thickness"       },
            {ShaderDataType::Int,       "a_EntityID"        },
            });

        s_Data.LineVertexArray->AddVertexBuffer(s_Data.LineVertexBuffer);
        s_Data.LineVertexBufferBase = new LineVertex[s_Data.MaxVertices];
        #pragma endregion


        //Sprites with textures
        s_Data.TextureVertexArray = VertexArray::Create();

        s_Data.TextureVertexBuffer = VertexBuffer::Create(s_Data.MaxVertices * sizeof(QuadTextureVertex));
        s_Data.TextureVertexBuffer->SetLayout({
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float4, "a_Color" },
            { ShaderDataType::Float2, "a_TexCoord" },
            { ShaderDataType::Float2,  "a_TilingFactor" },
            { ShaderDataType::Int,  "a_EntityID" },
            { ShaderDataType::Int, "a_TexIndex" }
            });

        s_Data.TextureVertexArray->AddVertexBuffer(s_Data.TextureVertexBuffer);
        s_Data.TextureVertexArray->SetIndexBuffer(quadIB);

        s_Data.TextureVertexBufferBase = new QuadTextureVertex[s_Data.MaxVertices];

        int32_t samplers[s_Data.MaxTextureSlots];
        for (uint32_t i = 0; i < s_Data.MaxTextureSlots; i++)
            samplers[i] = i;

        // Initialize the white texture
        uint32_t whiteTextureData = 0xffffffff;
        s_Data.WhiteTexture = Texture2D::Create("src/Renderer/BasicTextures/Test.png");

        //End of Textures

        s_Data.QuadShader = CreateRef<Shader>("assets/shaders/Renderer2D_Quad.glsl");
        s_Data.CircleShader = CreateRef<Shader>("assets/shaders/Renderer2D_Circle.glsl");
        s_Data.ColliderShader = CreateRef<Shader>("assets/shaders/Renderer2D_Quad.glsl");
        s_Data.CircleColliderShader = s_Data.CircleShader;
        s_Data.LineShader = CreateRef<Shader>("assets/shaders/Renderer2D_Line.glsl");

        s_Data.TextureShader = CreateRef<Shader>("assets/shaders/Renderer2DTexture.glsl");
        s_Data.TextureShader->Bind();
        s_Data.TextureShader->SetIntArray("u_Textures", samplers, s_Data.MaxTextureSlots);

        // Initialize texture slots
        s_Data.Textures[0] = s_Data.WhiteTexture;
        //
        //s_Data.QuadShader->Bind();
        //s_Data.QuadShader->SetInt("u_Texture", 0);

        s_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
        s_Data.QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
        s_Data.QuadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
        s_Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

        s_Data.CameraUniformBuffer = UniformBuffer::Create(sizeof(Renderer2DData::CameraData), 0);

        glEnable(GL_DEPTH_TEST);
    }

    void Renderer2D::Shutdown()
    {
        delete[] s_Data.QuadVertexBufferBase;
    }

    void Renderer2D::Clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Renderer2D::SetClearColor(const glm::vec4& color)
    {
        glClearColor(color.r, color.g, color.b, color.a);
    }

    void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform)
    {
        s_Data.CameraBuffer.ViewProjection = camera.GetProjection() * glm::inverse(transform);
        s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));
        
        StartBatch();
    }

    void Renderer2D::BeginScene(const EditorCamera& camera)
    {
        s_Data.CameraBuffer.ViewProjection = camera.GetViewProjection();
        s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(Renderer2DData::CameraData));

        StartBatch();
    }

    void Renderer2D::EndScene()
    {
        Flush();
    }

    void Renderer2D::StartBatch()
    {
        // Quads
        s_Data.QuadIndexCount = 0;
        s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;
        
        // Circles
        s_Data.CircleIndexCount = 0;
        s_Data.CircleVertexBufferPtr = s_Data.CircleVertexBufferBase;

        // Texture Sprite
        s_Data.TextureIndexCount = 0;
        s_Data.TextureVertexBufferPtr = s_Data.TextureVertexBufferBase;

        //Colliders
        s_Data.ColliderIndexCount = 0;
        s_Data.ColliderVertexBufferPtr = s_Data.ColliderVertexBufferBase;
        s_Data.CircleColliderIndexCount = 0;
        s_Data.CircleColliderVertexBufferPtr = s_Data.CircleColliderVertexBufferBase;


        //Line
        s_Data.LineCount = 0;
        s_Data.LineVertexBufferPtr = s_Data.LineVertexBufferBase;
    }

    void Renderer2D::Flush()
    {

        if (s_Data.QuadIndexCount)
        {
            uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.QuadVertexBufferPtr - (uint8_t*)s_Data.QuadVertexBufferBase);
            s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

            s_Data.QuadShader->Bind();

            s_Data.QuadVertexArray->Bind();
            uint32_t count = s_Data.QuadIndexCount ? s_Data.QuadIndexCount : s_Data.QuadVertexArray->GetIndexBuffer()->GetCount();
            glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);

            s_Data.Stats.DrawCalls++;
        }

        if (s_Data.CircleIndexCount)
        {
            uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.CircleVertexBufferPtr - (uint8_t*)s_Data.CircleVertexBufferBase);
            s_Data.CircleVertexBuffer->SetData(s_Data.CircleVertexBufferBase, dataSize);

            s_Data.CircleShader->Bind();

            s_Data.CircleVertexArray->Bind();
            uint32_t count = s_Data.CircleIndexCount ? s_Data.CircleIndexCount : s_Data.CircleVertexArray->GetIndexBuffer()->GetCount();
            glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);

            s_Data.Stats.DrawCalls++;


        }

        if (s_Data.TextureIndexCount)
        {
            uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.TextureVertexBufferPtr - (uint8_t*)s_Data.TextureVertexBufferBase);
            s_Data.TextureVertexBuffer->SetData(s_Data.TextureVertexBufferBase, dataSize);

            // Bind textures
            for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++)
                s_Data.Textures[i]->Bind(i);

            //enable transparancy and dont bother with depth
            glEnable(GL_BLEND);
            //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); //use a pre multiplied alpha
            //glDepthMask(GL_FALSE);
            glDisable(GL_DEPTH_TEST);  // Not needed for 2D

            s_Data.TextureShader->Bind();

            s_Data.TextureVertexArray->Bind();

            uint32_t count = s_Data.TextureIndexCount ? s_Data.TextureIndexCount : s_Data.TextureVertexArray->GetIndexBuffer()->GetCount();
            glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);

            s_Data.Stats.DrawCalls++;

            //before rendering other objects, disable the effect of transparent textures
            glDepthMask(GL_TRUE);
            glDisable(GL_BLEND);


        }

        if (s_Data.ColliderIndexCount)
        {
            //glDepthMask(GL_FALSE);
            glDisable(GL_DEPTH_TEST);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

            uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.ColliderVertexBufferPtr - (uint8_t*)s_Data.ColliderVertexBufferBase);
            s_Data.ColliderVertexBuffer->SetData(s_Data.ColliderVertexBufferBase, dataSize);

            s_Data.ColliderShader->Bind();

            s_Data.ColliderVertexArray->Bind();
            uint32_t count = s_Data.ColliderIndexCount ? s_Data.ColliderIndexCount : s_Data.ColliderVertexArray->GetIndexBuffer()->GetCount();
            glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);

            s_Data.Stats.DrawCalls++;

            //glDepthMask(GL_TRUE);
            glEnable(GL_DEPTH_TEST);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);



        }

        if (s_Data.CircleColliderIndexCount)
        {
            //glDepthMask(GL_FALSE);
            glDisable(GL_DEPTH_TEST);

            uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.CircleColliderVertexBufferPtr - (uint8_t*)s_Data.CircleColliderVertexBufferBase);
            s_Data.CircleColliderVertexBuffer->SetData(s_Data.CircleColliderVertexBufferBase, dataSize);

            s_Data.CircleColliderShader->Bind();

            s_Data.CircleColliderVertexArray->Bind();
            uint32_t count = s_Data.CircleColliderIndexCount ? s_Data.CircleColliderIndexCount : s_Data.CircleColliderVertexArray->GetIndexBuffer()->GetCount();
            glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);

            s_Data.Stats.DrawCalls++;

            //glDepthMask(GL_TRUE);
            glEnable(GL_DEPTH_TEST);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        }

        if (s_Data.LineCount)
        {
            uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.LineVertexBufferPtr - (uint8_t*)s_Data.LineVertexBufferBase);
            s_Data.LineVertexBuffer->SetData(s_Data.LineVertexBufferBase, dataSize);

            s_Data.LineShader->Bind();

            s_Data.LineVertexArray->Bind();
            uint32_t count = s_Data.LineCount ? s_Data.LineCount : s_Data.LineVertexArray->GetIndexBuffer()->GetCount();
            glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);

            s_Data.Stats.DrawCalls++;
        }
    }

    void Renderer2D::NextBatch()
    {
        Flush();
        StartBatch();
    }

    void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2 size, const glm::vec4 color)
    {
        DrawQuad({ position.x, position.y, 0.0f }, size, color);
    }

    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2 size, const glm::vec4 color)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
            glm::scale(glm::mat4(1.0f), glm::vec3(size.x, size.y, 1.0f));

        DrawQuad(transform, color);
    }

    void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4 color, int entityID)
    {
        constexpr size_t quadVertexCount = 4;
        const int textureIndex = 0;
        constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
        const glm::vec2 TillingFactor = { 1.0f, 1.0f };
        
        if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
        {
            NextBatch();
        }
        for (size_t i = 0; i < quadVertexCount; i++)
        {
            s_Data.QuadVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
            s_Data.QuadVertexBufferPtr->Color = color;
            s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];
            //s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
            s_Data.QuadVertexBufferPtr->TilingFactor = TillingFactor;
            s_Data.QuadVertexBufferPtr->EntityID = entityID;
            s_Data.QuadVertexBufferPtr++;
        }

        s_Data.QuadIndexCount += 6;
        s_Data.Stats.QuadCount++;
    }

    void Renderer2D::DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4 color, int entityID)
    {
        constexpr size_t quadVertexCount = 4;

        constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
        //const glm::vec2* textureCoords = texture->textCoords;
        const glm::vec2 TillingFactor = { tilingFactor, tilingFactor };

        if (s_Data.TextureIndexCount >= Renderer2DData::MaxIndices)
        {
            NextBatch();
        }

        int textureIndex = 0;
        // Assume textures are stored in s_Data.Textures array
        for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++)
        {
            if (s_Data.Textures[i].get() == texture.get())
            {
                textureIndex = i;
                break;
            }
        }

        if (textureIndex == 0)
        {
            if (s_Data.TextureSlotIndex >= Renderer2DData::MaxTextureSlots)
                NextBatch();

            textureIndex = s_Data.TextureSlotIndex;
            s_Data.Textures[s_Data.TextureSlotIndex] = texture;
            s_Data.TextureSlotIndex++;
        }

        for (size_t i = 0; i < quadVertexCount; i++)
        {
            s_Data.TextureVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
            s_Data.TextureVertexBufferPtr->Color = color;
            s_Data.TextureVertexBufferPtr->TexCoord = textureCoords[i];
            s_Data.TextureVertexBufferPtr->TilingFactor = TillingFactor;
            s_Data.TextureVertexBufferPtr->EntityID = entityID;
            s_Data.TextureVertexBufferPtr->TexIndex = textureIndex;
            s_Data.TextureVertexBufferPtr++;
        }

        s_Data.TextureIndexCount += 6;
        s_Data.Stats.QuadCount++;
    }


    void Renderer2D::DrawTriangle(const glm::mat4& transform, const glm::vec4 color, int entityID)
    {
        constexpr size_t triangleVertexCount = 3;

        // equalateral triangle vertices centered at the origin
        const glm::vec4 vertices[] = {
            glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),    // Top vertex
            glm::vec4(-0.866f, -0.5f, 0.0f, 1.0f), // Bottom left vertex
            glm::vec4(0.866f, -0.5f, 0.0f, 1.0f)   // Bottom right vertex
        };

        constexpr glm::vec2 textureCoords[] = { { 0.5f, 1.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f } };
        const glm::vec2 TillingFactor = { 1.0f, 1.0f };

        if (s_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
        {
            NextBatch();
        }

        int textureIndex = 0;

        for (size_t i = 0; i < triangleVertexCount; i++)
        {
            s_Data.QuadVertexBufferPtr->Position = transform * vertices[i];
            s_Data.QuadVertexBufferPtr->Color = color;
            s_Data.QuadVertexBufferPtr->TexCoord = textureCoords[i];            
            //s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
            s_Data.QuadVertexBufferPtr->TilingFactor = TillingFactor;
            s_Data.QuadVertexBufferPtr->EntityID = entityID;
            s_Data.QuadVertexBufferPtr++;
        }

        s_Data.QuadIndexCount += 3;
        s_Data.Stats.QuadCount++;
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2 size, const float zRotation, const glm::vec4 color)
    {
        DrawRotatedQuad({ position.x, position.y, 0.0f }, size, zRotation, color);
    }

    void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2 size, const float zRotation, const glm::vec4 color)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
            glm::rotate(glm::mat4(1.0f), glm::radians(zRotation), { 0.0f, 0.0f, 1.0f }) *
            glm::scale(glm::mat4(1.0f), glm::vec3(size.x, size.y, 1.0f));

        DrawQuad(transform, color);
    }

    void Renderer2D::DrawCircle(const glm::mat4& transform, const glm::vec4 color, float thickness, float fade, int entityID)
    {


        if (s_Data.CircleIndexCount >= Renderer2DData::MaxIndices)
        {
            NextBatch();
        }

        for (size_t i = 0; i < 4; i++)
        {
            s_Data.CircleVertexBufferPtr->WorldPosition = transform * s_Data.QuadVertexPositions[i];
            s_Data.CircleVertexBufferPtr->LocalPosition = s_Data.QuadVertexPositions[i] * 2.0f;
            s_Data.CircleVertexBufferPtr->Color = color;
            s_Data.CircleVertexBufferPtr->Thickness = thickness;
            s_Data.CircleVertexBufferPtr->Fade = fade;
            s_Data.CircleVertexBufferPtr->EntityID = entityID;
            s_Data.CircleVertexBufferPtr->radius = 5.0f;
            s_Data.CircleVertexBufferPtr++;
        }
            
        s_Data.CircleIndexCount += 6;
        s_Data.Stats.QuadCount++;
    }


    void Renderer2D::DrawSprite(const glm::mat4& transform, SpriteRendererComponent& src, int entityID)
    {
        if (src.texture)
            DrawQuad(transform, src.texture, src.tilingFactor, src.Colour, entityID);
        else
        {
            if (src.meshType == Triangle)
                DrawTriangle(transform, src.Colour, entityID);
            else
                DrawQuad(transform, src.Colour, entityID);
        }

        //if(src.meshType == SpriteType::Square)
        //    DrawQuad(transform, src.Colour, entityID);
        //
        //if(src.meshType == SpriteType::Triangle)
        //    DrawTriangle(transform, src.Colour, entityID);
    }


    void Renderer2D::DrawBoxCollider(const glm::mat4& transform, glm::vec4 color, BoxCollider2DComponent& src, int entityID)
    {
        constexpr size_t quadVertexCount = 4;
        const int textureIndex = 0;
        constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
        const glm::vec2 TillingFactor = { 1.0f, 1.0f };

        if (s_Data.ColliderIndexCount >= Renderer2DData::MaxIndices)
        {
            NextBatch();
        }
        for (size_t i = 0; i < quadVertexCount; i++)
        {
            s_Data.ColliderVertexBufferPtr->Position = transform * s_Data.QuadVertexPositions[i];
            s_Data.ColliderVertexBufferPtr->Color = color;
            s_Data.ColliderVertexBufferPtr->TexCoord = textureCoords[i];
            s_Data.ColliderVertexBufferPtr->TilingFactor = TillingFactor;
            s_Data.ColliderVertexBufferPtr->EntityID = entityID;
            s_Data.ColliderVertexBufferPtr++;
        }

        s_Data.ColliderIndexCount += 6;
        s_Data.Stats.QuadCount++;
    }

    void Renderer2D::DrawCircleCollider(const glm::mat4& transform, glm::vec4 color, CircleCollider2DComponent& src, int entityID)
    {
        constexpr size_t quadVertexCount = 4;
        const int textureIndex = 0;
        constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
        const glm::vec2 TillingFactor = { 1.0f, 1.0f };

        if (s_Data.ColliderIndexCount >= Renderer2DData::MaxIndices)
        {
            NextBatch();
        }
        for (size_t i = 0; i < 4; i++)
        {
            s_Data.CircleColliderVertexBufferPtr->WorldPosition = transform * s_Data.QuadVertexPositions[i];
            s_Data.CircleColliderVertexBufferPtr->LocalPosition = s_Data.QuadVertexPositions[i] * 2.0f;
            s_Data.CircleColliderVertexBufferPtr->Color = color;
            s_Data.CircleColliderVertexBufferPtr->Thickness = 0.05f;
            s_Data.CircleColliderVertexBufferPtr->Fade = 0.005f;
            s_Data.CircleColliderVertexBufferPtr->EntityID = entityID;
            s_Data.CircleColliderVertexBufferPtr->radius = src.Radius;
            s_Data.CircleColliderVertexBufferPtr++;
        }

        s_Data.CircleColliderIndexCount += 6;
        s_Data.Stats.QuadCount++;
    }

    void Renderer2D::DrawLine(const glm::vec3& origin, const glm::vec3& end, const glm::vec4 color, int entityID)
    {
        s_Data.LineVertexBufferPtr->Position = origin;
        s_Data.LineVertexBufferPtr->Color = color;
        s_Data.LineVertexBufferPtr->EntityID = entityID;
        s_Data.LineVertexBufferPtr++;

        s_Data.LineVertexBufferPtr->Position = end;
        s_Data.LineVertexBufferPtr->Color = color;
        s_Data.LineVertexBufferPtr->EntityID = entityID;
        s_Data.LineVertexBufferPtr++;

        s_Data.LineCount += 2;
    }

    float Renderer2D::GetLineWidth()
    {
        return s_Data.LineWidth;
    }

    void Renderer2D::SetLineWidth(float width)
    {
        s_Data.LineWidth = width;
    }

    void Renderer2D::DrawCollider(const glm::mat4& transform, BoxCollider2DComponent& src, int entityID)
    {
        if(src.type == ColliderTypes::Box)
            DrawBoxCollider(transform, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), src, entityID);

    }
    void Renderer2D::DrawColliderCircle(const glm::mat4& transform, CircleCollider2DComponent& src, int entityID)
    {
        if(src.type == ColliderTypes::Circle)
            DrawCircleCollider(transform, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), src, entityID);

    }

    std::vector<Ref<SubTexture2D>> Renderer2D::GenerateSpriteSheet(Ref<Texture2D> texture, int sprites, glm::vec2& Table, glm::vec2& spriteSize)
    {
        std::vector<Ref<SubTexture2D>> spriteSheet;
        for (int y = 0; y < Table.y; y++)
        {
            for (int x = 0; x < Table.x; x++)
            {
                glm::vec2 coords{ x, y };
                spriteSheet.push_back(SubTexture2D::CreateFromCoords(texture, coords, spriteSize));
            }
        }

        return spriteSheet;
    }

    void Renderer2D::ResetStats()
    {
        memset(&s_Data.Stats, 0, sizeof(Statistics));
    }

    Renderer2D::Statistics Renderer2D::GetStats()
    {
        return s_Data.Stats;
    }


    
}