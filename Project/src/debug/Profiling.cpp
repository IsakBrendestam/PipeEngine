#include "stdafx.h"

#ifdef TRACY_ENABLE

tracy::D3D11Ctx* g_tracyContext = nullptr;

void TracySetup()
{
	if (g_tracyContext)
		TracyD3D11Destroy(g_tracyContext);
	g_tracyContext = tracy::CreateD3D11Context(DeviceManager::Device(), DeviceManager::Context());
}

void TracyDestroy()
{
	TracyD3D11Destroy(g_tracyContext);
}

void TracyEndFrame()
{
	TracyD3D11Collect(g_tracyContext);
	FrameMark;
}

#endif
