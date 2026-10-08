#include "stdafx.h"

#include "Debug.h"

#pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS

#ifdef _DEBUG
void DebugLog(std::string x)
{
    if (DEBUG)
    {
        time_t now = std::time(0);
        tm* ltm = std::localtime(&now);

        std::string timestamp = "[" + std::to_string(5 + ltm->tm_hour) + ":" + std::to_string(ltm->tm_min) + ":" + std::to_string(ltm->tm_sec) + "] ";

        std::cout << timestamp << x << std::endl;
    }
}
#endif

void UpdateError()
{
	if (ImGui::BeginPopupModal("Error", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Error: Could not open file");
		if (ImGui::Button("OK"))
			ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}
}

