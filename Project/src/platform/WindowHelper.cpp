#include "stdafx.h"

#include "WindowHelper.h"
#include "systems/InputManager.h"
#include <iostream>
#include "imgui/imgui.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_LBUTTONDOWN:
		InputManager::SetMouseState(MouseButton::LeftButton, true);
		break;

	case WM_MBUTTONDOWN:
		InputManager::SetMouseState(MouseButton::MiddleButton, true);
		break;

	case WM_RBUTTONDOWN:
		InputManager::SetMouseState(MouseButton::RightButton, true);
		break;

	case WM_LBUTTONUP:
		InputManager::SetMouseState(MouseButton::LeftButton, false);
		break;

	case WM_MBUTTONUP:
		InputManager::SetMouseState(MouseButton::MiddleButton, false);
		break;

	case WM_RBUTTONUP:
		InputManager::SetMouseState(MouseButton::RightButton, false);
		break;

	case WM_MOUSEMOVE:
		InputManager::SetMousePosition(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_KEYDOWN:
	case WM_KEYUP:
	{
		uint32_t VKCode = wParam;

		bool wasDown = (lParam & (1 << 30)) != 0;
		bool isDown = (lParam & (1 << 31)) == 0;

		if (wasDown != isDown)
			InputManager::SetKeyState(VKCode, isDown);
	}
	break;
	}


	return DefWindowProc(hWnd, message, wParam, lParam);
}

bool SetupWindow(HINSTANCE instance, UINT width, UINT height, int nCmdShow, HWND& window, LPCWSTR windowText, bool createConsole)
{
	const wchar_t CLASS_NAME[] = L"Demo Window Class";

	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = instance;
	wc.lpszClassName = CLASS_NAME;

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	const unsigned int windowXPos = 600;
	const unsigned int windowYPos = 100;

	window = CreateWindowEx(0, CLASS_NAME, windowText, WS_OVERLAPPEDWINDOW, windowXPos, windowYPos, width, height, nullptr, nullptr, instance, nullptr);

	if (window == nullptr)
	{
		std::cerr << "HWND was nullptr, last error: " << GetLastError() << std::endl;
		MessageBox(0, L"HWND was nullptr", 0, 0);
		return false;
	}

	if (createConsole)
		CreateConsoleWindow(500, 120, 43, 60, 75, 100);

	ShowWindow(window, nCmdShow);

	return true;
}

void CreateConsoleWindow(int bufferLines, int bufferColumns, int windowLines, int windowColumns, int xPos, int yPos)
{
	// Console descriptor
	CONSOLE_SCREEN_BUFFER_INFO coninfo;

	// Get the console info and set the number of lines
	AllocConsole();
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
	coninfo.dwSize.Y = bufferLines;
	coninfo.dwSize.X = bufferColumns;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

	// Set the number of lines and columns
	SMALL_RECT rect;
	rect.Left = 0;
	rect.Top = 0;
	rect.Right = windowColumns;
	rect.Bottom = windowLines;
	SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &rect);

	// Redirect the standard input, output and error to the console
	FILE* stream;
	freopen_s(&stream, "CONIN$", "r", stdin);
	freopen_s(&stream, "CONOUT$", "w", stdout);
	freopen_s(&stream, "CONOUT$", "w", stderr);

	// Prevent accidental console window close
	HWND consoleHandle = GetConsoleWindow();
	HMENU hmenu = GetSystemMenu(consoleHandle, FALSE);
	EnableMenuItem(hmenu, SC_CLOSE, MF_GRAYED);

	// Set Position
	SetWindowPos(consoleHandle, 0, xPos, yPos, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	// Set Title
	SetConsoleTitle(L"Console Window");

	// Disable resizing
	LONG style = GetWindowLong(consoleHandle, GWL_STYLE);
	style = style & ~(WS_THICKFRAME);
	SetWindowLong(consoleHandle, GWL_STYLE, style);

	// Print Defualt message
	std::cout << "-------------------------------------------------------------" << std::endl;
	std::cout << "        Console Window for error and debug messages!         " << std::endl;
	std::cout << "-------------------------------------------------------------" << std::endl;
}
