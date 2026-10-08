#include "stdafx.h"

#include "InputManager.h"

MouseState InputManager::m_mouseState;
std::map<uint32_t, bool> InputManager::m_keyStates;

void InputManager::Update(SDL_Event event)
{
	switch (event.type)
	{

	case SDL_MOUSEMOTION:
		SetMouseDelta(event.motion.xrel, event.motion.yrel);
		break;

	case SDL_KEYDOWN:
		switch (event.key.keysym.sym)
		{
		case SDLK_F1:
			SDL_SetRelativeMouseMode(SDL_GetRelativeMouseMode() == SDL_TRUE ? SDL_FALSE : SDL_TRUE);
			break;
		default:
			break;
		}

		InputManager::SetKeyState(event.key.keysym.sym, true);
		break;

	case SDL_MOUSEBUTTONDOWN:
		switch (event.button.button)
		{
		case SDL_BUTTON_LEFT:
			SetMouseState(MouseButton::LeftButton, true);
			break;
		case SDL_BUTTON_RIGHT:
			SetMouseState(MouseButton::RightButton, true);
			break;
		default:
			break;
		}
		break;

	case SDL_MOUSEBUTTONUP:
		switch (event.button.button)
		{
		case SDL_BUTTON_LEFT:
			SetMouseState(MouseButton::LeftButton, false);
			break;
		case SDL_BUTTON_RIGHT:
			SetMouseState(MouseButton::RightButton, false);
			break;
		default:
			break;
		}
		break;

	case SDL_MOUSEWHEEL:
		m_mouseState.verticalScroll = static_cast<float>(event.wheel.y);
		break;

	case SDL_KEYUP:
		InputManager::SetKeyState(event.key.keysym.sym, false);
		break;

	default:
		break;
	}
}

bool InputManager::GetKeyState(uint32_t key)
{
	return m_keyStates[key];
}

void InputManager::SetKeyState(uint32_t key, bool state)
{
	m_keyStates[key] = state;
}

MouseState InputManager::GetMouseState()
{
	return m_mouseState;
}

void InputManager::SetMousePosition(int x, int y)
{
	m_mouseState.x = x;
	m_mouseState.y = y;
}

void InputManager::SetMouseDelta(float dx, float dy)
{
	m_mouseState.deltaX = dx;
	m_mouseState.deltaY = dy;
}

void InputManager::SetMouseState(MouseButton button, bool state)
{
	switch (button)
	{
	case MouseButton::LeftButton:
		m_mouseState.leftButtonDown = state;
		break;
	case MouseButton::RightButton:
		m_mouseState.rightButtonDown = state;
		break;
	case MouseButton::MiddleButton:
		m_mouseState.middleButtonDown = state;
		break;
	}
}

void InputManager::SetMouseScroll(float vertical)
{
	m_mouseState.verticalScroll = vertical;
}

void InputManager::LockMouse()
{
	SDL_SetRelativeMouseMode(SDL_TRUE);
}

void InputManager::UnlockMouse()
{
	SDL_SetRelativeMouseMode(SDL_FALSE);
}
