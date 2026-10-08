#include "stdafx.h"

#include "WindowManager.h"

#include "imgui/imgui_impl_sdl2.h"
#include <SDL_mouse.h>

//UINT WindowManager::clientWidth = 960, WindowManager::clientHeight = 960;
UINT WindowManager::clientWidth = 1600, WindowManager::clientHeight = 850;
UINT WindowManager::windownWidth = WindowManager::clientWidth, WindowManager::windowsHeight = WindowManager::clientHeight;


std::vector<std::pair<UINT, UINT>> WindowManager::s_predefinedResolutions = {
	std::make_pair(1920, 1080),
	std::make_pair(1280, 720),
	std::make_pair(640, 480)
};

struct WindowInformation
{
	UINT windowWidth;
	UINT windowHeight;
	int mouseX;
	int mouseY;
	UINT mouseState;
	int temp[3];
} windowInformation;

WindowManager::~WindowManager()
{
	delete m_windowBuffer;

	SDL_DestroyWindow(m_window);
	SDL_Quit();
}

int WindowManager::InitializeWindow()
{
	m_running = true;

	windowInformation.windowWidth = WindowManager::windownWidth;
	windowInformation.windowHeight = WindowManager::windowsHeight;
	windowInformation.mouseX = 0;
	windowInformation.mouseY = 0;

#ifdef USE_SDL
	if (DEBUG)
		CreateConsoleWindow(500, 120, 43, 60, 75, 100);
	return InitializeSDL2();

#endif

#ifdef USE_WIN32
	return InitializeWin32(NULL, 0);
#endif
}

void WindowManager::LateInit()
{
    m_windowBuffer = new ConstantBufferD3D11(sizeof(WindowInformation), true, &windowInformation);
}

#ifdef USE_SDL
int WindowManager::InitializeSDL2()
{
	if (SDL_Init(SDL_INIT_VIDEO))
		return 0;

	m_window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WindowManager::windownWidth, WindowManager::windowsHeight, SDL_WINDOW_SHOWN );
	SDL_SetWindowResizable(m_window, SDL_TRUE);
	SDL_VERSION(&m_sysInfo.version);
	SDL_GetWindowWMInfo(m_window, &m_sysInfo);
	SDL_SetRelativeMouseMode(SDL_FALSE);

	return 1;
}
#endif

#ifdef USE_WIN32
int WindowManager::InitializeWin32(HINSTANCE hInstance, int nCmdShow)
{
	if (!SetupWindow(hInstance, WindowManager::windownWidth, WindowManager::windowsHeight, nCmdShow, m_window, L"Project", DEBUG))
	{
		std::cerr << "Failed to setup window!" << std::endl;
		return -1;
	}

	RECT clientRect;
	GetClientRect(m_window, &clientRect);
	WindowManager::clientWidth = clientRect.right;
	WindowManager::clientHeight = clientRect.bottom;

	DebugLog("Window initialized successfully!");

	return 0;
}
#endif

void WindowManager::UpdateWindow()
{
#ifdef USE_WIN32
	// Message Handler
	while (PeekMessage(&m_msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&m_msg);
		DispatchMessage(&m_msg);

		if (m_msg.message != VM_QUIT)
			m_running = false;
	}
#endif

#ifdef USE_SDL
	SDL_GetMouseState(&windowInformation.mouseX, &windowInformation.mouseY);

	InputManager::SetMousePosition(windowInformation.mouseX, windowInformation.mouseY);
	InputManager::SetMouseDelta(0, 0);
	InputManager::SetMouseScroll(0);

	
	while (SDL_PollEvent(&m_event))
	{
		switch (m_event.type)
		{
		case SDL_QUIT:
			m_running = false;
			break;
		
		case SDL_WINDOWEVENT:
			switch (m_event.window.event) 
			{
			case SDL_WINDOWEVENT_CLOSE:
				m_running = false;
				break;

			default:
				break;
			}
			break;

		default:
			break;
		}

		InputManager::Update(m_event);

		//if (g_engineSettings.imguiOn)
			ImGui_ImplSDL2_ProcessEvent(&m_event);
	}
#endif

	m_windowBuffer->UpdateBuffer(&windowInformation);
}

bool WindowManager::IsRunning() const
{
	return m_running;
}

void WindowManager::MinimizeWindow()
{
#ifdef USE_SDL
	SDL_MinimizeWindow(m_window);
#endif
}

#ifdef USE_SDL
SDL_Window* WindowManager::GetSDLWindow()
{
	return m_window;
}
#endif // USE_SDL


HWND WindowManager::GetWindow()
{
#ifdef USE_WIN32
	return m_window;
#endif

#ifdef USE_SDL
	return m_sysInfo.info.win.window;
#endif

}

void WindowManager::BindBuffer()
{
	const unsigned int nPSBuff = 1;
	ID3D11Buffer* psBuffers[nPSBuff] = { m_windowBuffer->GetBuffer() };
	DeviceManager::Context()->PSSetConstantBuffers(0, nPSBuff, psBuffers);
}
