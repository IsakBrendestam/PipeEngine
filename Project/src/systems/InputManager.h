#pragma once

#include <map>
#include <iostream>
#include <Windows.h>

enum MouseButton
{
	LeftButton = 0,
	RightButton = 1,
	MiddleButton = 2,
};

struct MouseState
{
	int x, y;
	float deltaX, deltaY;
	bool leftButtonDown, rightButtonDown, middleButtonDown;
	float verticalScroll;
};

class InputManager
{
private:
	static MouseState m_mouseState;
	static std::map<uint32_t, bool> m_keyStates;

public:
	static void Update(SDL_Event event);

	static bool GetKeyState(uint32_t key);
	static void SetKeyState(uint32_t key, bool state);

	static MouseState GetMouseState();
	static void SetMousePosition(int x, int y);
	static void SetMouseDelta(float dx, float dy);
	static void SetMouseState(MouseButton button, bool state);
	static void SetMouseScroll(float vertical);

	static void LockMouse();
	static void UnlockMouse();
};