#pragma once

#if 1
#define USE_SDL
#else
#define USE_WIN32
#endif

#include <SDL.h>
#include <SDL_syswm.h>
#include <wtypes.h>
#include <vector>

#include "core/rendering/d3d11/buffers/ConstantBufferD3D11.h"

#include "core/rendering/d3d11/DeviceManager.h"

#include "platform/WindowHelper.h"
#include "debug/Debug.h"

class WindowManager 
{
public:
	static UINT windownWidth, windowsHeight;
	static UINT clientWidth, clientHeight;

	static std::vector<std::pair<UINT, UINT>> s_predefinedResolutions;
	
	WindowManager() = default;
	~WindowManager();

	int InitializeWindow();
	void LateInit();
	int InitializeWin32(HINSTANCE hInstance, int nCmdShow);
	int InitializeSDL2();

	void UpdateWindow();
	bool IsRunning() const;

	void MinimizeWindow();

#ifdef USE_SDL
	SDL_Window* GetSDLWindow();
#endif
	HWND GetWindow();

	void BindBuffer();

private:
	ConstantBufferD3D11* m_windowBuffer = nullptr;

	bool m_running = true;

#ifdef USE_SDL
	SDL_Window* m_window = NULL;
	SDL_SysWMinfo m_sysInfo = {};
	SDL_Event m_event = {};
#endif

#ifdef USE_WIN32
	MSG m_msg = {};
	HWND m_window = 0;
#endif
};