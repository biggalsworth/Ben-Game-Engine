#pragma once
#include "Engine_PCH.h"
#include "Project.h"
#include "Script/MonoManager.h"

namespace Engine
{
	std::filesystem::path Project::currCodeProj;
	std::unique_ptr<filewatch::FileWatch<std::wstring>> Project::watch;


	Project::Project()
	{
		s_TextureLibrary = CreateRef<TextureLibrary>();
		currCodeProj = std::filesystem::current_path() / L"../Projects/GameDemo";
		if (std::filesystem::exists(currCodeProj))
		{
			std::cout << "Loaded code project: " << currCodeProj.lexically_normal().string() << std::endl;
		}

		// This will watch the scritps path and notify of changes
		std::filesystem::path scriptPath = Project::currCodeProj / L"Assets/Scripts";

		watch = std::make_unique<filewatch::FileWatch<std::wstring>>(
			scriptPath.wstring(),
			[](const std::wstring& path, const filewatch::Event change_type)
			{
				std::filesystem::path scriptPath = Project::currCodeProj / L"Assets/Scripts";
				std::wcout << L"\nDetected Change: " << path << L" - ";
				std::wcout << std::filesystem::path(scriptPath / path) << L"\n";

				switch (change_type) {
				case filewatch::Event::added:
					std::wcout << L"\nFile Added" << std::endl;
					break;
				case filewatch::Event::removed:
					std::wcout << L"File Removed" << std::endl;
					break;
				case filewatch::Event::modified:
					std::wcout << L"File Modified" << std::endl;
					break;
				case filewatch::Event::renamed_old:
					std::wcout << L"File Renamed (Old)" << std::endl;
					break;
				case filewatch::Event::renamed_new:
					std::wcout << L"File Renamed (New)" << std::endl;
					break;
				}
			}
		);

	}

	Project::~Project()
	{

	}
}

