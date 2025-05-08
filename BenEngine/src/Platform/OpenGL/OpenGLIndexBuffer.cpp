#include "Engine_PCH.h"
#include "OpenGLIndexBuffer.h"
#include <glad/glad.h>

namespace Engine
{
	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count) : m_Count(count)
	{
		glCreateBuffers(1, &m_RendererID);

		//GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
		//building with GL_ARRAY_BUFFER allows data to laod regardless of VAO
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);

		//static draw needs to be modified later
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);

	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void OpenGLIndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	}

	void OpenGLIndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}