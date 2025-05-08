#include "Engine_PCH.h"
#include "WindowsWindow.h"

#include <Events/ApplicationEvent.h>
#include <Events/KeyEvent.h>
#include <Events/MouseEvent.h>

#include "GraphicsAPI/OpenGL/OpenGLContext.h"


#define STB_IMAGE_IMPLEMENTATION
#include "GraphicsAPI/stb_image.h"

namespace Engine
{
	// Function to load image data
	bool LoadImageData(const char* filePath, std::vector<unsigned char>& imageData, int& width, int& height)
	{
		int channels;
		unsigned char* pixels = stbi_load(filePath, &width, &height, &channels, STBI_rgb_alpha);
		if (!pixels)
		{
			std::cerr << "Failed to load image: " << filePath << std::endl;
			return false;
		}

		imageData.assign(pixels, pixels + (width * height * 4));
		stbi_image_free(pixels);
		return true;
	}

	void setWindowIcon(GLFWwindow* window, const char* filePath)
	{
		int width, height;
		std::vector<unsigned char> imageData;

		if (LoadImageData(filePath, imageData, width, height))
		{
			GLFWimage images[1];
			images[0].width = width;
			images[0].height = height;
			images[0].pixels = imageData.data();

			glfwSetWindowIcon(window, 1, images);
		}
	}

	static uint8_t s_GLFWWindowCount = 0;

	static void GLFWErrorCallback(int errorCode, const char* errorMessage)
	{
		//std::cout << "GLFW ERROR: (" << errorCode << ") - \nMessage: " << errorMessage << std::endl;
		LOG_ERROR("GLFW ERROR: ({0}) \nMessage: {1}", errorCode, errorMessage);
	}

	//create a window class - using windows operating system
	std::unique_ptr<Window> Window::Create(const WindowProps& props)
	{
		return std::make_unique<WindowsWindow>(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		Init(props);
	}
	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}

	void WindowsWindow::Init(const WindowProps& props)
	{
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		LOG_INFO("Create Window {0} ({1}, {2})", props.Title, props.Width, props.Height);

		if (s_GLFWWindowCount == 0)
		{
			//no window is open
			int success = glfwInit();

			glfwSetErrorCallback(GLFWErrorCallback);
		}
		{
			m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
			++s_GLFWWindowCount;

			setWindowIcon(m_Window, "src/Extra/Icons/Logo/LionIcon.jpg");
		}

		//make the context this window and initialise this context for rendering
		m_Context = new OpenGLContext(m_Window);
		m_Context->Init();

		glfwSetWindowUserPointer(m_Window, &m_Data);

		SetVSync(true);

		//set GLFW callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				data.Width = width;
				data.Height = height;

				WindowResizeEvent ev(width, height);

				data.EventCallback(ev);
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				WindowCloseEvent ev;
				data.EventCallback(ev);
			});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action) 
				{
				case GLFW_PRESS:
				{
					KeyPressedEvent ev(key, 0);
					data.EventCallback(ev);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent ev(key);
					data.EventCallback(ev);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent ev(key, 1);
					data.EventCallback(ev);
					break;
				}
				}
			});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				KeyTypedEvent ev(keycode);
				data.EventCallback(ev);
			});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				switch(action)
				{
				
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent ev(button);
					data.EventCallback(ev);
					break;
				}

				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent ev(button);
					data.EventCallback(ev);
					break;
				}
				}
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				MouseScrolledEvent ev((float)xOffset, (float)yOffset);
				data.EventCallback(ev);
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				MouseMovedEvent ev((float)xPos, (float)yPos);
				data.EventCallback(ev);
			});
	}

	void WindowsWindow::Shutdown()
	{
		glfwDestroyWindow(m_Window);
		--s_GLFWWindowCount;

		if (s_GLFWWindowCount == 0)
		{
			glfwTerminate();
		}
	}

	void WindowsWindow::OnUpdate()
	{
		glfwPollEvents();
		m_Context->SwapBuffers(); //make the context the rendered layer
	}

	void WindowsWindow::SetVSync(bool enabled) 
	{
		if (enabled)
		{
			glfwSwapInterval(1);
		}
		else
		{
			glfwSwapInterval(0);
		}
		m_Data.VSync = enabled;
	}

	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;
	}
}