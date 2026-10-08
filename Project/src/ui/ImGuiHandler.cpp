#include "stdafx.h"

#include "ImGuiHandler.h"

#include "core/rendering/d3d11/DeviceManager.h"

ImGuiIO* ImGuiHandler::m_io;
std::vector<float> ImGuiHandler::m_fpsData;
float ImGuiHandler::m_timeSinceFpsAdd = 0;
bool ImGuiHandler::m_vsyncOn = false;
WindowAPI ImGuiHandler::m_windowAPI = WindowAPI::NULL_API;
bool ImGuiHandler::m_initialized = false;

void ImGuiHandler::SetupWin32(HWND hwnd)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls    
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    m_io = &io;

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(DeviceManager::Device(), DeviceManager::Context());

    m_windowAPI = WindowAPI::Win32;

    m_initialized = true;
}

void ImGuiHandler::SetupSDL(SDL_Window* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls    
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    m_io = &io;

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForD3D(window);
    ImGui_ImplDX11_Init(DeviceManager::Device(), DeviceManager::Context());

    m_windowAPI = WindowAPI::SDL2;

    m_initialized = true;
}

void ImGuiHandler::Deconstruct()
{
    if (m_initialized)
    {
        ImGui_ImplDX11_Shutdown();

        switch (m_windowAPI)
        {
        case Win32:
            ImGui_ImplWin32_Shutdown();
            break;
        case SDL2:
            ImGui_ImplSDL2_Shutdown();
            break;
        case NULL_API:
            break;
        default:
            break;
        }

        ImGui::DestroyContext();
    }
}

void ImGuiHandler::SetNewFrame()
{
	ZoneScopedC(RandomUniqueColor());

    ImGui_ImplDX11_NewFrame();
    switch (m_windowAPI)
    {
    case Win32:
        ImGui_ImplWin32_NewFrame();
        break;
    case SDL2:
        ImGui_ImplSDL2_NewFrame();
        break;
    case NULL_API:
        break;
    default:
        break;
    }
    ImGui::NewFrame();

	ImGui::DockSpaceOverViewport(0, NULL, ImGuiDockNodeFlags_PassthruCentralNode);
}

void ImGuiHandler::Render()
{
	ZoneScopedC(RandomUniqueColor());

    ID3D11RenderTargetView* rtv = DeviceManager::WindowRTV();
    static const float clearColor[4] = {0};
	DeviceManager::Context()->ClearRenderTargetView(rtv, clearColor);
    DeviceManager::Context()->OMSetRenderTargets(1, &rtv, nullptr);

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    if (m_io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}


void ImGuiHandler::ScreenStatistics()
{
	ZoneScopedC(RandomUniqueColor());

    const float deltaTime = g_time.DeltaTime();
    const float fps = (float)g_time.FPS();

    ImGui::Begin("Statistics");

    {
	    ZoneScopedNC("Standard Statistics", RandomUniqueColor());

		ImGui::Text("Initialization Time: %.0f s", g_time.InitTime());
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", deltaTime, fps);

		m_timeSinceFpsAdd += deltaTime;

		if (m_timeSinceFpsAdd > 0.5)
		{
			if (m_fpsData.size() < 20)
			{
				m_fpsData.push_back(fps);
			}
			else
			{
				m_fpsData.erase(m_fpsData.begin());
				m_fpsData.push_back(fps);
			}

			m_timeSinceFpsAdd = 0;
		}

		ImGui::PlotLines("FPS rates", m_fpsData.data(), m_fpsData.size());

    }

    {
	    ZoneScopedNC("Scoped Timers", RandomUniqueColor());

		ImGui::SeparatorText("Scoped Timers");

		for (const auto& scope : g_time.GetAllAverageScopeTimes())
		{
			double avg = scope.second.sum / scope.second.empirical.size();
			ImGui::Text("%s: %.3fms", scope.first.c_str(), avg);
		}

		if (ImGui::Button("Reset Timers"))
			g_time.ResetAverageTimes();
		ImGui::SameLine();
		if (ImGui::DragInt("Sample count", (int*)&ScopeData::maxSamples, 1.0f, 1, 10000))
			g_time.ResetAverageTimes();
    }

    ImGui::End();
}

void ImGuiHandler::InformationPanel()
{
	ZoneScopedC(RandomUniqueColor());

    static const unsigned int nKeybinds = 10;
    static std::string names[nKeybinds] = {
        "Move Forward",
        "Move Backwards",
        "Move Left",
        "Move Right",
        "Move Up",
        "Move Down",
        "Look Up",
        "Look Down",
        "Look Left",
        "Look Right"
    };

    static std::string keys[nKeybinds] = {
        "W",
        "S",
        "A",
        "D",
        "SPACE",
        "CTRL",
        "UP ARROW",
        "DOWN ARROW",
        "LEFT ARROW",
        "RIGHT ARROW"
    };

    ImGui::Begin("Information");

    ImGui::TextWrapped("This application has been developed by Isak Brendestam.");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Key Bindings"))
    {
        static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
        if (ImGui::BeginTable("table1", 2, flags))
        {
            // Headers
            ImGui::TableSetupColumn("Name");
            ImGui::TableSetupColumn("Key");
            ImGui::TableHeadersRow();

            for (int i = 0; i < nKeybinds; i++)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(names[i].c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(keys[i].c_str());
            }

            ImGui::EndTable();
        }
    }

    ImGui::End();
}

