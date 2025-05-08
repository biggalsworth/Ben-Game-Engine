#include "Engine_PCH.h"
#include "ConsolePanel.h"
#include <imgui.h>

namespace Engine
{
	ConsolePanel::ConsolePanel()
	{
	}

	void ConsolePanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
	}

	void ConsolePanel::Render(ConsoleBuffer& buffer)
	{

		ImGui::PushFont(UI::Fonts["righteous-small"]); //fonts are compiled when in ImGuilayer.OnAttach()
		// Display each line in the buffer
		for (const auto& line : buffer.lines) {
			if(line != "")
				ImGui::Text(line.c_str());
		}
		ImGui::PopFont();

		// Automatic scrolling
		if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
			ImGui::SetScrollHereY(1.0f);
	}
}