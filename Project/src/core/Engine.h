#pragma once

#include "EngineSettings.h"

#include "scene/Scene.h"
#include "physics/PhysicsEngine.h"

namespace DX = DirectX;

class Engine
{
public:
	Engine(HINSTANCE hInstance, int nCmdShow);
	virtual ~Engine();

	int Run();

private:
	int Initialize(HINSTANCE hInstance, int nCmdShow);
	void Deconstruct();

	void RenderWindowUI();

	void SetScene(Scene*& scene);
	const Scene* GetScene();

	void Update();
	void Render();

private:
	int m_initCode;
	double m_initTime;
	std::chrono::time_point<std::chrono::steady_clock> m_initStart;
	std::chrono::time_point<std::chrono::steady_clock> m_initEnd;

	HWND m_window;
	UINT clientWidth, clientHeight;
	bool m_renderWindowFocused = true;

	WindowManager* m_windowManager = nullptr;

	Scene* m_scene = nullptr;
	std::vector<Scene*> m_scenes;

	PhysicsEngine m_physicsEngine;
};

