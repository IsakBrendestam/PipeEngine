#pragma once

#include <Windows.h>
#include "systems/InputManager.h"

bool SetupWindow(HINSTANCE instance, UINT width, UINT height, int nCmdShow, HWND& widnow, LPCWSTR windowText, bool createConsole = false);

void CreateConsoleWindow(int bufferLines, int bufferColumns, int windowLines, int windowColumns, int xPos, int yPos);