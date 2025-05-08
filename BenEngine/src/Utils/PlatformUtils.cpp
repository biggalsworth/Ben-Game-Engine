#include "Engine_PCH.h"
#include "PlatformUtils.h"
#include <filesystem>

#include "Platform/Windows/WindowsPlatformUtils.h"

namespace Engine
{
	std::string FileDialogs::OpenFile(const char* filter)
	{
		return WindowsFileDialogs::OpenFile(filter);
	}


	std::string FileDialogs::SaveFile(const char* filter)
	{
		return WindowsFileDialogs::SaveFile(filter);
	}



	std::filesystem::path FileSystem::CreateFolder(std::filesystem::path directory, std::string folderName)
	{
		std::filesystem::path filePath = directory / folderName;

		if (!std::filesystem::exists(filePath)) {
			if (std::filesystem::create_directories(filePath)) {
				std::cout << "Folder created: " << filePath << std::endl;
			}
			else {
				std::cerr << "Failed to create folder!" << std::endl;
			}
		}
		

		return filePath;
	}

	std::filesystem::path FileSystem::CreateNewFile(std::filesystem::path directory, std::string fileName, std::string data)
	{
		std::filesystem::path filePath = directory / fileName;
		if (std::filesystem::exists(filePath))
		{
			std::cout << "File Already Exists! " << filePath << std::endl;
			return NULL;
		}
		std::ofstream outFile(filePath);
		if (outFile) 
		{
			outFile << data << std::endl;
			std::cout << "File created successfully: " << filePath << std::endl;

		}
		else 
		{
			std::cerr << "Failed to create file!" << std::endl;
			return NULL;
		}

		return filePath;
	}

	std::filesystem::path FileSystem::RenameFolder(std::filesystem::path directory, std::string newName)
	{
		if (!std::filesystem::exists(directory))
		{
			return NULL;
		}
		LOG_INFO("FILE : " + directory.filename().string());

		return directory;
	}

	std::filesystem::path FileSystem::FindRelativeToProject(std::filesystem::path directory)
	{
		std::filesystem::path fullPath = directory;
		std::string baseFolder = "GameEngine-12129991";

		// Convert path to string for easy manipulation
		std::string fullPathStr = fullPath.string();

		// Find where "GameEngine-12129991" starts in the path
		size_t pos = fullPathStr.find(baseFolder);
		if (pos != std::string::npos) {
			std::string relativePath = fullPathStr.substr(pos); // Extract starting from "GameEngine-12129991"
			std::cout << "Relative Path: " << relativePath << std::endl;
			return std::filesystem::path(relativePath);

		}
		else {
			std::cout << "Base folder not found in path!" << std::endl;
		}
	}

}