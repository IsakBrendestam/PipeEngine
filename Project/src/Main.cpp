#include "stdafx.h"

#include <Windows.h>
#include "core/Engine.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	if (DEBUG)
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	ZoneScopedNC("3D Game Engine", RandomUniqueColor());

	Engine engine(hInstance, nCmdShow);

	return engine.Run();
}
