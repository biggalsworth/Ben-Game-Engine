#pragma once
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

namespace Engine
{
	class FileDialogs
	{
	public:
		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);

	};

	class FileSystem
	{

	public:
		static std::filesystem::path CreateFolder(std::filesystem::path directory, std::string folderName);
		static std::filesystem::path CreateNewFile(std::filesystem::path directory, std::string fileName, std::string data = "");

		static std::filesystem::path RenameFolder(std::filesystem::path directory, std::string newName);
		static std::filesystem::path FindSystemPath(std::string localPath);
		static std::filesystem::path FindRelativeToProject(std::filesystem::path directory);
	};


	class SystemDialogue
	{
	public:
		static bool Warning(const std::string& message);
	};
}