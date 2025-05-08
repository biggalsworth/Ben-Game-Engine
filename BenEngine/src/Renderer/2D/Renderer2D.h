#pragma once
#include "Renderer/Camera/Camera.h"
#include "Renderer/Camera/EditorCamera.h"

#include "Scene/Components.h"

namespace Engine
{

	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void Clear();
		static void SetClearColor(const glm::vec4& color);

		static void BeginScene(const EditorCamera& camera);
		static void BeginScene(const Camera&, const glm::mat4& transform);
		static void EndScene();

		static void Flush();

		static void DrawQuad(const glm::vec2& position, const glm::vec2 size, const glm::vec4 color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2 size, const glm::vec4 color);
		static void DrawQuad(const glm::mat4& transform, const glm::vec4 color, int entityID = -1);
		static void DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture, float tilingFactor, const glm::vec4 color, int entityID);

		//static void DrawTriangle(const glm::mat4& transform, const glm::vec4 color);
		static void DrawTriangle(const glm::mat4& transform, const glm::vec4 color, int entityID);

		static void DrawRotatedQuad(const glm::vec2& position, const glm::vec2 size, const float zRotation, const glm::vec4 color);
		static void DrawRotatedQuad(const glm::vec3& position, const glm::vec2 size, const float zRotation, const glm::vec4 color);

		static void DrawCircle(const glm::mat4& transform, const glm::vec4 color, float thickness = 1.0f, float fade = 0.005f, int entityID = -1);

		static void DrawSprite(const glm::mat4& transform, SpriteRendererComponent& src, int entityID);


		static void DrawBoxCollider(const glm::mat4& transform, glm::vec4 color, BoxCollider2DComponent& src, int entityID);
		static void DrawCircleCollider(const glm::mat4& transform, glm::vec4 color, CircleCollider2DComponent& src, int entityID);

		static void DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4 color, int entityID);

		static float GetLineWidth();

		static void SetLineWidth(float width);

		static void DrawCollider(const glm::mat4& transform, BoxCollider2DComponent& src, int entityID);
		static void DrawColliderCircle(const glm::mat4& transform, CircleCollider2DComponent& src, int entityID);

		static std::vector<Ref<SubTexture2D>> GenerateSpriteSheet(Ref<Texture2D> texture, int sprites, glm::vec2& Table, glm::vec2& spriteSize);


		struct Statistics
		{
			uint32_t DrawCalls = 0; //how many draw calls this frame
			uint32_t QuadCount = 0; //how many quads rendered in the current frame

			uint32_t GetTotalVertexCount() { return QuadCount * 4; }
			uint32_t GetTotalIndexCount() { return QuadCount * 6; }
		};

		static void ResetStats();
		static Statistics GetStats();

	private:
		static void StartBatch();
		static void NextBatch();
	};
}