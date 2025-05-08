#pragma once

//this is just defining the abstract class from which the graphics API context will inherit - e.g. OpenGLContext.h

namespace Engine
{
	class GraphicsContext
	{
	public:
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
	};
}