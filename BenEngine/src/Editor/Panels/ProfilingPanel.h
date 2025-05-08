#pragma once
#include <psapi.h>
#include <pdh.h>
#include "ImPlot/implot.h"

namespace Engine
{

	struct ProfilingData
	{

		int MaxFPS = 500; //the number of frames to compare to the current usage
		int MaxCPU = 100; //the max percentage of CPU usage show

		int MaxMemory = 100; //the max percentage of memory show
		//Data variables
		float fpsDataHistory[1200] = { 0 };	// History of FPS values, going back 1,200 entries
		int fpsHistoryIndex = 0;			// Current index in the history buffer
		float fpsPercentageHistory[120] = { 0 };
		float cpuDataHistory[120] = { 0 };
		float memoryHistory[120] = { 0 };
		float memoryPercentageHistory[120] = { 0 };
		int historyIndex = 0;

		float cpuUsage;
		float memoryUsage;

		bool showFPS;
		bool showCPU;
		bool showMemory;

	};

	class ProfilingPanel
	{
	public:
		ProfilingPanel();
		~ProfilingPanel()
		{
			StopRecording();
		};

	public:
		void Render();
		void RenderFullGraph();
		void SetMaxFPS(int newMax);
		float CPUStats();
		float MemoryStats();

		void StartRecording() 
		{
			record = true;
			//std::thread threadHandler(&ProfilingPanel::RecordThread, this, std::ref(showCPU), std::ref(showMemory));
			std::thread threadHandler([this]() { RecordThread(std::ref(s_Data)); });
			threadHandler.detach();
		}
		void StopRecording() { record = false;  };

	private:
		void RecordThread(ProfilingData& data);

		float GetMaxAvailableMemory()
		{
			MEMORYSTATUSEX memInfo;
			memInfo.dwLength = sizeof(memInfo);

			if (GlobalMemoryStatusEx(&memInfo))
			{
				return static_cast<float>(memInfo.ullTotalPhys) / (1024 * 1024); // Convert the total available memory to MB
				//return static_cast<float>(memInfo.ullTotalPhys) / (1024 * 1024 * 1024); // Convert to GB
			}

			return 0.0f;  // Return 0 if retrieval fails
		}


	public:
		
		static ProfilingData s_Data;

		bool show = false;

		bool record = false;

		float MaxAvailableMemory;

	private:
		std::thread threadHandler;

	};
}
	