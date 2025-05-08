#include "Engine_PCH.h"
#include "ProfilingPanel.h"

namespace Engine
{

	//float ProfilingPanel::fpsDataHistory[1200] = { 0 };  // History of FPS values, going back 1,200 entries
	//int ProfilingPanel::	fpsHistoryIndex = 0;
	//float ProfilingPanel::cpuDataHistory[120] = { 0 };
	//float ProfilingPanel::memoryHistory[100] = { 0 };
	//int ProfilingPanel::	historyIndex = 0;

	ProfilingData ProfilingPanel::s_Data = ProfilingData();

	ProfilingPanel::ProfilingPanel()
	{

		s_Data.showFPS = false;
		s_Data.showCPU = false;
		s_Data.showMemory = false;
		MaxAvailableMemory = GetMaxAvailableMemory();
		s_Data.MaxMemory = (int)std::round(MaxAvailableMemory / 10); //start by showing a less than the max memory available.
	}

	void ProfilingPanel::Render()
	{

		//record the fps history
		s_Data.fpsDataHistory[s_Data.fpsHistoryIndex] = ImGui::GetIO().Framerate;
		s_Data.fpsHistoryIndex = (s_Data.fpsHistoryIndex + 1) % 1200;  // Circular buffer

		ImGui::Begin("Profiler");
		ImGui::BeginTable("Individual", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY, ImVec2{ ImGui::GetContentRegionAvail().x, 200 });
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();

			ImGui::Text("Profiling Information:");


			ImGui::Checkbox("FPS", &s_Data.showFPS);
			//ImGui::SameLine();
			ImGui::Checkbox("CPU", &s_Data.showCPU);
			//ImGui::SameLine();
			ImGui::Checkbox("Memory", &s_Data.showMemory);

			ImGui::TableNextColumn();

			if (s_Data.showFPS) {

				ImGui::SeparatorText("Frame Rate:");

				ImGui::PlotLines(" ", s_Data.fpsDataHistory, IM_ARRAYSIZE(s_Data.fpsDataHistory), s_Data.fpsHistoryIndex, ("Max: " + std::to_string(s_Data.MaxFPS) + " frames").c_str(),
					0.0f, static_cast<float>(s_Data.MaxFPS), ImVec2(ImGui::GetContentRegionAvail().x, 80));
				ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

				// Slider to change max frames
				ImGui::SliderInt("Max Frames", &s_Data.MaxFPS, 10, 2000, "Max: %d frames");

			}

			if (s_Data.showCPU)
			{
				ImGui::SeparatorText("CPU Usage:");
				ImGui::PlotLines("CPU%", s_Data.cpuDataHistory, IM_ARRAYSIZE(s_Data.cpuDataHistory), s_Data.historyIndex, "",
					0.0f, static_cast<float>(s_Data.MaxCPU), ImVec2(ImGui::GetContentRegionAvail().x, 80));
				ImGui::Text("Current CPU Usage: %.1f%%", s_Data.cpuUsage);

				ImGui::SliderInt("Max CPU", &s_Data.MaxCPU, 10, 100, "%d: Max CPU Usage shown");

			}

			if (s_Data.showMemory)
			{
				ImGui::SeparatorText("Memory Usage:");

				char buffer[64];  // Allocate space
				sprintf(buffer, "Max Available Memory: %.2f MB", MaxAvailableMemory);
				std::string maxMemoryLabel(buffer); // Convert to std::string


				ImVec2 graphSize = ImVec2(ImGui::GetContentRegionAvail().x, 80);
				ImVec2 graphPos = ImGui::GetCursorScreenPos(); // Get graph position on screen

				ImGui::PlotLines("Memory (MB)", s_Data.memoryHistory, IM_ARRAYSIZE(s_Data.memoryHistory), s_Data.historyIndex, maxMemoryLabel.c_str(),
					0.0f, static_cast<float>(s_Data.MaxMemory), ImVec2(ImGui::GetContentRegionAvail().x, 80));

				ImGui::Text("Current Memory Usage: %.2f MB", s_Data.memoryUsage);
				ImGui::SliderInt("Max GBs shown", &s_Data.MaxMemory, 10, (int)std::round(MaxAvailableMemory), "%d: GBs of memory shown");

			}
		
			ImGui::EndTable();
		}

		ImGui::Separator();
		RenderFullGraph();

		//1200 is the scale of the graph
		s_Data.historyIndex = (s_Data.historyIndex + 1) % 1200; // Circular buffer index
		ImGui::End();

	}

	//static float t = 1;

	void ProfilingPanel::RenderFullGraph()
	{
		//static float history = 100.0f;
		//static float count = 100.0f;
		//static float xscale = 100.0f;

		//testing sliders
		//ImGui::SliderFloat("History", &history, 1, 2000, "%.1f s");
		//ImGui::SliderFloat("Count", &count, 1, 2000, "%.1f s");
		//ImGui::SliderFloat("X Scale", &xscale, 1, 2000, "%.1f s");

		static ImPlotAxisFlags flags = ImPlotAxisFlags_NoTickLabels;
		//int   bar_data[11] = 
		//float x_data[120] = ;
		//float y_data[s_Data] = ...;
		//t += ImGui::GetIO().DeltaTime;
		if (ImPlot::BeginPlot("##Scrolling", ImVec2(-1, 200))) 
		{
			ImPlot::SetupAxes( NULL, "Percentage", flags, ImPlotAxisFlags_Lock | ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoLabel);
			ImPlot::SetupAxisLimits(ImAxis_X1, 0, 120, ImGuiCond_Always);
			ImPlot::SetupAxisLimits(ImAxis_Y1, -0.5, 100); //100 is max percentage
			ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.5f);

			//plot data
			ImPlot::PlotLine("FPS Percentage", &s_Data.fpsPercentageHistory[0], 120, 1.0);
			ImPlot::PlotLine("Total CPU Usage", &s_Data.cpuDataHistory[0], 119, 1.0);
			ImPlot::PlotLine("Application Memory Usage", &s_Data.memoryPercentageHistory[0], 119, 1.0);


			if (ImPlot::IsPlotHovered()) {
				ImGui::BeginTooltip();
				int hoveredIndex = ImPlot::GetPlotMousePos().x;  // Get closest point
				hoveredIndex = std::clamp(hoveredIndex, 0, 120); // Prevent out-of-bounds errors

				ImGui::Text("FPS percentage: %.2f%%", s_Data.fpsPercentageHistory[hoveredIndex]);
				ImGui::Text("CPU Usage: %.2f%%", s_Data.cpuDataHistory[hoveredIndex]);
				ImGui::Text("Memory Usage: %.2f%%", s_Data.memoryPercentageHistory[hoveredIndex]);
				ImGui::EndTooltip();
			}


			ImPlot::EndPlot();
		}

	}

	void ProfilingPanel::SetMaxFPS(int newMax)
	{
		s_Data.MaxFPS = newMax;
	}

	float ProfilingPanel::CPUStats()
	{
		//set the base variables
		PDH_HQUERY query;
		PDH_HCOUNTER counter;
		PDH_FMT_COUNTERVALUE value;

		// Initialize query
		PdhOpenQuery(NULL, 0, &query);
		PdhAddEnglishCounter(query, L"\\Processor(_Total)\\% Processor Time", 0, &counter); //Total cpu usage

		PdhCollectQueryData(query);
		Sleep(100);  // Waits for 1 second to collect  data
		PdhCollectQueryData(query);		
		//PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, NULL, &value);
		PDH_STATUS status = PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, NULL, &value);
		if (status != ERROR_SUCCESS) {
			std::cerr << "Failed to get CPU stats: " << status << std::endl;
		}

		PdhCloseQuery(query);

		return static_cast<float>(value.doubleValue);
	}

	float ProfilingPanel::MemoryStats()
	{
		//the memory variables
		PROCESS_MEMORY_COUNTERS_EX pmc;

		pmc.cb = sizeof(pmc);

		if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
			return static_cast<float>(pmc.PrivateUsage) / (1024 * 1024);  // Memory usage in MB
		}
		return 0.0f;  // Return 0 if no memory info is found
	}

	void ProfilingPanel::RecordThread(ProfilingData& data)
	{
		const int SMOOTHING_WINDOW = 1;  // the section size for smoothing

		while (record)
		{
			if (s_Data.historyIndex < IM_ARRAYSIZE(s_Data.fpsPercentageHistory)) {
				s_Data.fpsPercentageHistory[s_Data.historyIndex] = ImGui::GetIO().Framerate / s_Data.MaxFPS * 100.0f;
			}
			else {
				memmove(s_Data.fpsPercentageHistory, s_Data.fpsPercentageHistory + 1, (IM_ARRAYSIZE(s_Data.fpsPercentageHistory) - 1) * sizeof(float));
				s_Data.fpsPercentageHistory[IM_ARRAYSIZE(s_Data.fpsPercentageHistory) - 1] = ImGui::GetIO().Framerate / s_Data.MaxFPS * 100.0f;
			}

			//Record CPU
			if (data.showCPU)
			{
				//float cpuSum = 0.0f;
				////s_Data.cpuUsage = CPUStats();
				//for (int i = 0; i < SMOOTHING_WINDOW; i++)
				//{
				//	float currentCpu = CPUStats();
				//	cpuSum += currentCpu;
				//}

				//s_Data.cpuUsage = cpuSum / SMOOTHING_WINDOW;  // Compute moving average so the times between CPU reads arent as obvious
				s_Data.cpuUsage = CPUStats();  // Compute moving average so the times between CPU reads arent as obvious

				if (s_Data.historyIndex < IM_ARRAYSIZE(s_Data.cpuDataHistory)) {
					s_Data.cpuDataHistory[s_Data.historyIndex] = s_Data.cpuUsage;
				}
				else {
					memmove(s_Data.cpuDataHistory, s_Data.cpuDataHistory + 1, (IM_ARRAYSIZE(s_Data.cpuDataHistory) - 1) * sizeof(float));
					s_Data.cpuDataHistory[IM_ARRAYSIZE(s_Data.cpuDataHistory) - 1] = s_Data.cpuUsage;
				}
			}

			//Record Memory
			if (s_Data.showMemory)
			{
				s_Data.memoryUsage = MemoryStats();
				// Store memory usage in history buffer
				if (s_Data.historyIndex < IM_ARRAYSIZE(s_Data.memoryHistory)) {
					s_Data.memoryHistory[s_Data.historyIndex] = s_Data.memoryUsage; //get the MBs of used memory
					s_Data.memoryPercentageHistory[s_Data.historyIndex] = s_Data.memoryUsage / MaxAvailableMemory * 100.0f; //get the percentage of used memory

				}
				else {
					memmove(s_Data.memoryHistory, s_Data.memoryHistory + 1, (IM_ARRAYSIZE(s_Data.memoryHistory) - 1) * sizeof(float));
					s_Data.memoryHistory[IM_ARRAYSIZE(s_Data.memoryHistory) - 1] = s_Data.memoryUsage;
					s_Data.memoryPercentageHistory[IM_ARRAYSIZE(s_Data.memoryPercentageHistory) - 1] = s_Data.memoryUsage / MaxAvailableMemory * 100.0f; //get the percentage
				}
			}
		}
	}

}