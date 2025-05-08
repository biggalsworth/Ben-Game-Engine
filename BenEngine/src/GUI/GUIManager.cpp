#include "Engine_PCH.h"
#include "GUIManager.h"
#include <GLFW/glfw3.h>

namespace Engine
{
	GUIManager::GUIManager()
	{
	}

	//void GUIManager::Init(glm::vec2 windowSize)
	//{
	//	sf::View guiView(sf::FloatRect(0, 0, windowSize.x, windowSize.y));
	//	guiView.setCenter(windowSize.x / 2.0f, windowSize.y / 2.0f); // Center the view on the screen
	//	guiView.setSize(windowSize.x, windowSize.y); // Set the size to match the window
	//
	//	sf::Font font;
	//	if (!font.loadFromFile("src/Editor/Fonts/righteous/Righteous-Regular.ttf")) {
	//		// Handle error if font is not found
	//		std::cerr << "Error loading font!" << std::endl;
	//		return;
	//	}
	//
	//	sf::Text text;
	//	text.setFont(sf::Font());
	//	text.setString("This is a 2D overlay");
	//	text.setCharacterSize(24);
	//	text.setFillColor(sf::Color::Transparent);
	//
	//}
}