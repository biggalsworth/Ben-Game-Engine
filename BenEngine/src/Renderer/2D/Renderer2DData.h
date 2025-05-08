#pragma once
#include "Renderer2D.h"

#include "Renderer/Data/VertexArray.h"
#include "Renderer/Shader/Shader.h"
#include "Renderer/Data/UniformBuffer.h"

#include <Renderer/Data/Primatives/QuadVertex.h>
#include <Renderer/Data/Primatives/CircleVertex.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Renderer/Data/Primatives/LineVertex.h>

namespace Engine
{
	struct Renderer2DData
	{

        static const uint32_t MaxQuads = 20000;
        static const uint32_t MaxVertices = MaxQuads * 4;
        static const uint32_t MaxIndices = MaxQuads * 6;
        static const uint32_t MaxTextureSlots = 32;

        glm::vec4 QuadVertexPositions[4];


        // Quads
        Ref<VertexArray> QuadVertexArray; // Create the VAO - binds and manages vertex and index buffers for rendering quads
        Ref<VertexBuffer> QuadVertexBuffer; // Create the VBO - stores vertex data for quads
        Ref<Shader> QuadShader;

        // Tracks indices in the current batch
        uint32_t QuadIndexCount = 0;

        QuadVertex* QuadVertexBufferBase = nullptr; // The base pointer to the CPU-side buffer
        QuadVertex* QuadVertexBufferPtr = nullptr; // A pointer to the current position in the CPU buffer for new vertices


        //ColliderQuads
            Ref<VertexArray> ColliderVertexArray; // Create the VAO - binds and manages vertex and index buffers for rendering quads
            Ref<VertexBuffer> ColliderVertexBuffer; // Create the VBO - stores vertex data for quads

            Ref<VertexArray> CircleColliderVertexArray;   
            Ref<VertexBuffer> CircleColliderVertexBuffer; 

            uint32_t ColliderIndexCount = 0;
            uint32_t CircleColliderIndexCount = 0;

            Ref<Shader> ColliderShader;
            Ref<Shader> CircleColliderShader;

            QuadVertex* ColliderVertexBufferBase = nullptr; // The base pointer to the CPU-side buffer
            QuadVertex* ColliderVertexBufferPtr = nullptr; // A pointer to the current position in the CPU buffer for new vertices

            CircleVertex* CircleColliderVertexBufferBase = nullptr; // The base pointer to the CPU-side buffer
            CircleVertex* CircleColliderVertexBufferPtr = nullptr; // A pointer to the current position in the CPU buffer for new vertices

        //Circles
        Ref<VertexArray> CircleVertexArray;
        Ref<VertexBuffer> CircleVertexBuffer;
        Ref<Shader> CircleShader;

        uint32_t CircleIndexCount = 0;
        CircleVertex* CircleVertexBufferBase = nullptr;
        CircleVertex* CircleVertexBufferPtr = nullptr;


        //Lines
        Ref<VertexArray> LineVertexArray;
        Ref<VertexBuffer> LineVertexBuffer;
        Ref<Shader> LineShader;

        uint32_t LineCount = 0;
        float LineWidth = 10;
        LineVertex* LineVertexBufferBase = nullptr;
        LineVertex* LineVertexBufferPtr = nullptr;


        //Sprite With Texture
        Ref<VertexArray> TextureVertexArray;
        Ref<VertexBuffer> TextureVertexBuffer;
        Ref<Shader> TextureShader;

        uint32_t TextureIndexCount = 0;
        QuadTextureVertex* TextureVertexBufferBase = nullptr;
        QuadTextureVertex* TextureVertexBufferPtr = nullptr;

        Ref<Texture2D> WhiteTexture; // A default white texture

        uint32_t TextureSlotIndex = 1; // Slot 0 is reserved for the white texture
        std::array<Ref<Texture2D>, MaxTextureSlots> Textures; // Array of textures

        Renderer2D::Statistics Stats;

        struct CameraData
        {
            glm::mat4 ViewProjection;
        };
        CameraData CameraBuffer;
        Ref<UniformBuffer> CameraUniformBuffer;
	};
}