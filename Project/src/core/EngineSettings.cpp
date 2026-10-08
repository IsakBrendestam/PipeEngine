#include "stdafx.h"

#include "EngineSettings.h"

EngineSettings g_engineSettings = {
    false,                      // vsyncOn
    false,                      // wireFrameOn
    false,                      // deferredRenderingOn
    true,                       // frustumCullingOn
    true,                       // dynamicEnvMapOn
    true,                       // renderGizmos
    false,                      // cameraMouseMovement
    {0.5f, 0.5f, 0.5f, 0.0f},   // clearColor
    //{WindowManager::clientWidth, WindowManager::clientHeight} //renderResolution
    //{1280, 720},                //renderResolution
    {1920, 1080},               //renderResolution
    //{3840, 2160},               //renderResolution
    true,                       // physicsEnabled
};
