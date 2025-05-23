#include "Engine_PCH.h"
#include "PlatformUtils.h"
#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#endif

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

	//std::filesystem::path FileSystem::FindSystemPath(std::filesystem::path localPath)
	std::filesystem::path FileSystem::FindSystemPath(std::string localPath)
	{

		std::filesystem::path systemPath = std::filesystem::current_path();

		std::filesystem::path baseFolder = systemPath.parent_path().parent_path() / (std::filesystem::path)localPath;

		if(std::filesystem::exists(baseFolder))
			return baseFolder;
		else
		{
			std::cout << ("file not found: " + localPath) << std::endl;
			return localPath;
		}
	}

	std::filesystem::path FileSystem::FindRelativeToProject(std::filesystem::path directory)
	{
		std::filesystem::path fullPath = directory;
		std::filesystem::path localpath = std::filesystem::current_path();

		//finds the engine folder, even if changed name
		std::string baseFolder = localpath.parent_path().filename().string();

		// Convert path to string for easy manipulation
		std::string fullPathStr = fullPath.string();

		// Find where the GameEngine project folder starts in the path
		size_t pos = fullPathStr.find(baseFolder);
		if (pos != std::string::npos) {
			std::string relativePath = fullPathStr.substr(pos); // Extract starting from base path
			std::cout << "Relative Path: " << relativePath << std::endl;
			return std::filesystem::path(relativePath);

		}
		else {
			std::cout << "Base folder not found in path!" << std::endl;
		}
	}



	std::wstring StringToWString(const std::string& str)
	{
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
		std::wstring wstr(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size_needed);
		return wstr;
	}

	bool SystemDialogue::Warning(const std::string& message)
	{
		#ifdef _WIN32


		std::wstring wString = StringToWString(message);


		// Windows: Use MessageBox
		int result = MessageBox(NULL,
			wString.c_str(),
			StringToWString("Are you sure you want to proceed?").c_str(),
			MB_YESNO | MB_ICONWARNING);
		if (result == IDYES)
		{
			return true;
		}
		else
		{
			return false;
		}
		#else
		// Linux (GTK-based systems): Use Zenity dialog
		int result = system("zenity --question --text='You are about to delete this file. Proceed?' --title='Warning'");
		if (result == 0) // Zenity returns 0 if 'Yes' is clicked
		{
			std::cout << "User confirmed action.\n";
			return true;

		}
		else
		{
			std::cout << "User confirmed action.\n";
			return false;

		}
		#endif

	}

}