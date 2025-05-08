#pragma once
#include "Engine_PCH.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Engine
{
	struct LineVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		float Thickness;

		int EntityID;
	};
}
