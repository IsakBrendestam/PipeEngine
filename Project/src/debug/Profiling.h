#pragma once

#include "debug/ScopeTimer.h"
#include "debug/ScopeTimerGPU.h"

/*
#ifdef _DEBUG
	#include "debug/ScopeTimer.h"
	#include "debug/ScopeTimerGPU.h"
#else
	#define InternalScope(_)
	#define InternalScopeA(_, __)
	#define InternalScopeGPU(_)
#endif
*/

#ifdef TRACY_ENABLE
	#include "Tracy.hpp"
	#include "TracyD3D11.hpp"

	extern tracy::D3D11Ctx* g_tracyContext;

	void TracySetup();
	void TracyDestroy();

	void TracyEndFrame();
#else
	#define ZoneScopedNCD(_, __)
	#define ZoneScopedC(_)
	#define ZoneScopedNC(_, __)

	#define TracyD3D11ZoneC()
	#define TracyD3D11Collect()

	#define FrameMark
#endif


