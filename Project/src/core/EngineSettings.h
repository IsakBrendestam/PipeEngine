#pragma once

#include <chrono>
#include "debug/Debug.h"

static const auto g_START_TIME = std::chrono::high_resolution_clock::now();

class EngineSettings
{
public:
	bool vsyncOn;
	bool wireFrameOn;
	bool deferredRenderingOn;
	bool frustumCullingOn;
	bool dynamicEnvMapOn;
	bool renderGizmos;
	bool cameraMouseMovement;
	float clearColor[4];
	UINT renderResolution[2];

	bool physicsEnabled;

	const bool imguiOn = true && DEBUG;
};

extern EngineSettings g_engineSettings;
