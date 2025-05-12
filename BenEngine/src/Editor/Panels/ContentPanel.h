#pragma once
#include "Core/Core.h"
#include "Logging/Log.h"
#include "Scene/Scene.h"
#include "Utils/PlatformUtils.h"

namespace Engine
{
	class ContentPanel
	{
	public:
		ContentPanel();
		~ContentPanel() = default;

		void Setup();
		void SetContext(const Ref<Scene>& context);

		void ListDirectoryContents(const std::filesystem::path& directoryPath);
		void ListFolders(const std::filesystem::path& directoryPath);
		void ShowSettingsPanel();
		void Render();

		void RightClickMenu();


		// Function to scan files in a directory
		static std::vector<std::string> ScanFiles(const std::filesystem::path& directory)
		{
			std::vector<std::string> fileList;
			fileList.clear();
			for (const auto& entry : std::filesystem::directory_iterator(directory)) {
				//if (!entry.is_directory()) { // Only show files
					fileList.push_back(entry.path().filename().string());
				//}
			}

			return fileList;
		}

	private:
		Ref<Scene> m_Context;
		std::filesystem::path m_SelectedPath;
		std::string m_SelectedFile;
		std::filesystem::path m_CurrentDirectory;
		bool deleting;

		//float* ThumbnailSize = new float(128.0f);
		//float* Padding = new float(16.0f);
		//bool* ShowPhysicsColliders = false;

	};
}