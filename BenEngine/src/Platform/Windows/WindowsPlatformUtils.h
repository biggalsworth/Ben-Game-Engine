#pragma once
#include "Utils/PlatformUtils.h"

namespace Engine
{

	class WindowsFileDialogs : FileDialogs
	{
	public:
		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);
	};

}