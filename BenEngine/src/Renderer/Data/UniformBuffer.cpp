#include "Engine_PCH.h"
#include "UniformBuffer.h"

#include "Platform/OpenGL/OpenGLUniformBuffer.h"

namespace Engine
{
	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
	{
		return CreateRef<OpenGLUniformBuffer>(size, binding);
	}
}