#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

#include "imgui/imgui_impl_sdl2.h"

#include <d3d11_4.h>
#include <tchar.h>
#include <vector>
#include <string>

enum WindowAPI
{
	Win32,
	SDL2,
	NULL_API
};

class ImGuiHandler
{
public:
	static void SetupWin32(HWND hwnd);
	static void SetupSDL(SDL_Window* window);
	static void Deconstruct();

	static void SetNewFrame();
	static void Render();

	static void ScreenStatistics();

	static void InformationPanel();
private:
	static std::vector<float> m_fpsData;
	static float m_timeSinceFpsAdd;
	static bool m_vsyncOn;
	static ImGuiIO* m_io;

	static WindowAPI m_windowAPI;

	static bool m_initialized;
};