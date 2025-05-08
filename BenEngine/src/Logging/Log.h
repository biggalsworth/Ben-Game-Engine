#pragma once
#include "Core/Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Engine
{
	class Log
	{
	public:
		static void Init();
		inline static std::shared_ptr<spdlog::logger>& GetLogger() { return s_Logger; }
		
	private:
		static std::shared_ptr<spdlog::logger> s_Logger;
	};
}

//LOG SHORTCUTS
// 
//these are shorcuts so we dont have to type Log::GetLogger()->TYPE each 
//time we need to log to console
#define LOG_TRACE(...) :: Engine::Log::GetLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...) :: Engine::Log::GetLogger()->info(__VA_ARGS__)
#define LOG_WARN(...) :: Engine::Log::GetLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...) :: Engine::Log::GetLogger()->error(__VA_ARGS__)
#define LOG_FATAL(...) :: Engine::Log::GetLogger()->fatal(__VA_ARGS__)	