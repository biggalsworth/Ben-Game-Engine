#include "Engine_PCH.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include "glad/glad.h"

namespace Engine
{
	//constructor
	//this constructor will set the opengl context associated with m_windowHandle
	//Setting this will allow us to perform rendering operations
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle) : m_windowHandle(windowHandle)
	{

	}


	void OpenGLContext::Init() //initialise opengl context for the given window
	{
		//sets the opengl context for rendering
		glfwMakeContextCurrent(m_windowHandle);

		//Glad function - gets pointers to all modern opengl functions from GPU driver
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		
		CORE_ASSERT("{0} - Failed to initialise Glad!", status);
		
		LOG_INFO("Vendor: {0}", (const char*)glGetString(GL_VENDOR));
		LOG_INFO("Renderer: {0}", (const char*)glGetString(GL_RENDERER));
		LOG_INFO("Version OpenGL: {0}", (const char*)glGetString(GL_VERSION));
		LOG_INFO("Version GLSL: {0}", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

		//error checking
		//if (status != 1)
		//{
		//	std::cout << status << " - Failed to initialise Glad!" << std::endl;
		//}
	}

	//swap the active buffer to display the rendered frame
	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_windowHandle);
	}
}