#pragma once
#include "Engine_PCH.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Engine
{
	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		glm::vec2 TilingFactor;

		//editor-only
		int EntityID;
	};
	
	struct QuadTextureVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		glm::vec2 TilingFactor;

		//editor-only
		int EntityID;

		int TexIndex;

	};
}