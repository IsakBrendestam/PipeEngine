#include "stdafx.h"

#include "Engine.h"

#include "scenes/SceneSponza.h"
#include "scenes/SceneBenchmark.h"
#include "Scenes/SceneCTR.h"
#include "Scenes/SceneSobel.h"
#include "Scenes/SceneBenDay.h"
#include "Scenes/SceneField.h"
#include "Scenes/SceneAbsolution.h"
#include "Scenes/SceneStressTest.h"
#include "Scenes/SceneDepthOfField.h"
#include "Scenes/SceneTopology.h"
#include "Scenes/SceneCollision.h"
#include "scenes/SceneBloom.h"
#include "scenes/SceneGaussian.h"
#include "scenes/SceneTonemapping.h"

#include "ui/ImGuiHandler.h"

Engine::Engine(HINSTANCE hInstance, int nCmdShow)
{
	m_initTime = clock();
	m_initStart = std::chrono::steady_clock::now();

	m_initCode = Initialize(hInstance, nCmdShow);
}

Engine::~Engine()
{
	Deconstruct();

#ifdef TRACY_ENABLE
	TracyDestroy();
#endif

}

int Engine::Initialize(HINSTANCE hInstance, int nCmdShow)
{
	ZoneScopedC(RandomUniqueColor());

	m_windowManager = new WindowManager();
	int errorCode = m_windowManager->InitializeWindow();

	m_window = m_windowManager->GetWindow();

	DebugLog("Window initialized successfully!");

	RECT clientRect;
	GetClientRect(m_window, &clientRect);
	WindowManager::clientWidth = clientRect.right;
	WindowManager::clientHeight = clientRect.bottom;

	// Setup D3D11
	UINT w = g_engineSettings.renderResolution[0], 
		 h = g_engineSettings.renderResolution[1];

	//if (g_engineSettings.imguiOn)
	{
		w = WindowManager::clientWidth;
		h = WindowManager::clientHeight;
	}

	DeviceManager::Initialize(m_window, w, h);

	DebugLog("D3D11 initialized successfully!");


#ifdef TRACY_ENABLE
		TracySetup();
#endif

	g_cameraManager.Initialize();

	DebugLog("Pipeline initialized successfully!");


	DebugLog("Engine initialized successfully!");

	// Late Window Inti
	m_windowManager->LateInit();

	//g_threadPool.SetupPool(MAX_PROCESSOR_COUNT);

	return 0;
}

void Engine::Deconstruct()
{
	ZoneScopedC(RandomUniqueColor());

	g_cameraManager.Deconstruct();

	//if (g_engineSettings.imguiOn)
		ImGuiHandler::Deconstruct();

	delete m_windowManager;

	for (auto& scene : m_scenes)
		delete scene;


	DeviceManager::Deconstruct();
}

void Engine::RenderWindowUI()
{
	ZoneScopedC(RandomUniqueColor());

	ImGui::Begin("Render Output", nullptr, ImGuiWindowFlags_MenuBar);

	// Select with right click
	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows))
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			ImGui::SetWindowFocus();

	m_renderWindowFocused = ImGui::IsWindowFocused();

	if (g_engineSettings.imguiOn && ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Settings"))
		{
			ImGui::MenuItem("(Settings)");

			if (ImGui::BeginMenu("Scene Selection"))
			{
				for (auto& scene : m_scenes)
				{
					bool current = scene == GetScene();
					if (ImGui::MenuItem(scene->GetName().c_str(), 0, current) && !current)
						SetScene(scene);
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Resolutions"))
			{
				for (int i = 0; i < WindowManager::s_predefinedResolutions.size(); i++)
				{
					std::string name = std::format("{}x{}", WindowManager::s_predefinedResolutions[i].first, WindowManager::s_predefinedResolutions[i].second);
					if (ImGui::MenuItem(name.c_str()))
						std::cout << "Test" << std::endl;
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Camera"))
			{
				for (auto& id : g_cameraManager.GetCameraIds())
				{
					std::string name = std::format("Camera Id:{}", id);
					if (id == g_cameraManager.GetDefaultCameraId())
						name += " (default)";

					bool current = id == g_cameraManager.GetCameraId();
					if (ImGui::MenuItem(name.c_str(), 0, current) && !current)
					{
						g_cameraManager.SetControllCamera(id);
						g_cameraManager.SetCurrentCamera(id);
					}
				}

				ImGui::Separator();

				if (ImGui::Button("Add New Camera"))
					g_cameraManager.AddCamera(g_cameraManager.GenerateDefaultCamera());

				// Projection settings
				CameraD3D11* currentCamera = g_cameraManager.GetCamera();

				if (currentCamera != g_cameraManager.GetDefaultCamera())
				{
					ProjectionInfo projection = currentCamera->GetProjection();

					bool changed = false;
					float fov = projection.fovAngleY * GameMath::RAD_TO_DEG;
					changed |= ImGui::DragFloat("Fov", &fov, 0.1f, 0.1f, 90.0f);
					float planes[2]{ projection.nearZ, projection.farZ };
					changed |= ImGui::DragFloat2("Clipping", planes, 0.1f, 0.1f, 1000);

					if (changed)
					{
						if (planes[0] > planes[1])
							planes[0] = planes[1];

						projection.fovAngleY = fov * GameMath::DEG_TO_RAD;
						projection.nearZ = planes[0];
						projection.farZ = planes[1];

						currentCamera->SetProjection(projection);
					}
				}

				ImGui::EndMenu();
			}

			if (ImGui::Button("Open Mesh Inspector"))
				m_scene->OpenMeshInspector();

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	{
		RenderTargetD3D11* rtv = DeviceManager::RenderRTV();

		float texWidth  = (float)rtv->GetWidth();
		float texHeight = (float)rtv->GetHeight();
		float texAspect = texWidth / texHeight;

		ImVec2 avail = ImGui::GetContentRegionAvail();
		float availAspect = avail.x / avail.y;

		ImVec2 drawSize;

		if (availAspect > texAspect)
		{
			drawSize.y = avail.y;
			drawSize.x = drawSize.y * texAspect;
		}
		else
		{
			drawSize.x = avail.x;
			drawSize.y = drawSize.x / texAspect;
		}

		ImVec2 cursor = ImGui::GetCursorPos();
		ImGui::SetCursorPos({
			cursor.x + (avail.x - drawSize.x) * 0.5f,
			cursor.y + (avail.y - drawSize.y) * 0.5f
		});

		ImGui::Image((ImTextureID)rtv->GetSRV(), drawSize);
	}

	ImGui::End();
}

void Engine::SetScene(Scene*& scene)
{
	if (!scene) return;
	if (m_scene) m_scene->ExternalUnload();

	m_scene = scene;

	DebugLog("Changed to scene: " + m_scene->GetName());

	m_scene->ExternalLoad();

	m_physicsEngine.Initialize(scene);
}

const Scene* Engine::GetScene()
{
	return m_scene;
}

int Engine::Run()
{
	ZoneScopedC(RandomUniqueColor());

	const wchar_t* className = L"dewcin_window";
	WNDCLASS windowClass = {};

	if (m_initCode != 0)
		return m_initCode;

	g_resourceManager.LoadDefaultContent();

	m_scenes.push_back(new SceneBenchmark());		// 0
	m_scenes.push_back(new SceneStressTest());		// 1
	m_scenes.push_back(new SceneSobel());			// 2
	m_scenes.push_back(new SceneCollision());		// 3
	m_scenes.push_back(new SceneAbsolution());		// 4
	m_scenes.push_back(new SceneDepthOfField());	// 5
	m_scenes.push_back(new SceneTopology());		// 6
	m_scenes.push_back(new SceneField());			// 7
	m_scenes.push_back(new SceneCTR());				// 8
	m_scenes.push_back(new SceneBenDay());			// 9
	m_scenes.push_back(new SceneSponza());			// 10
	m_scenes.push_back(new SceneBloom());			// 11
	m_scenes.push_back(new SceneGaussian());		// 12
	m_scenes.push_back(new SceneTonemapping());		// 13

	for (auto& scene : m_scenes)
		scene->Initialize();

	SetScene(m_scenes[11]);

	DebugLog("Application initialized successfully!");

	//if (g_engineSettings.imguiOn)
		ImGuiHandler::SetupSDL(m_windowManager->GetSDLWindow());

	DebugLog("ImGui initialized successfully!");

	MSG msg = { };

	double avgDeltaTime = 0;
	double sumDeltaTime = 0;
	double avgCount = 0;

	unsigned int frameCount = 0;

	m_initEnd = std::chrono::steady_clock::now();
	m_initTime = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(m_initEnd - m_initStart).count();

	auto start{ std::chrono::steady_clock::now() };

	while (m_windowManager->IsRunning() && !InputManager::GetKeyState(SDLK_ESCAPE))
	{
		frameCount++;
		ZoneScopedNCD(std::format("Frame {}", frameCount).c_str(), RandomUniqueColor());
		InternalScope("Frame Time");

		m_windowManager->UpdateWindow();

		Update();

		Render();

		// Present rendered result
		{
			ZoneScopedNC("Present", RandomUniqueColor());
			DeviceManager::SwapChain()->Present(int(g_engineSettings.vsyncOn), 0);
		}

#ifdef TRACY_ENABLE
		TracyEndFrame();
#endif

		// Calulate Runtime Statistics

		const auto end{ std::chrono::steady_clock::now() };
		g_time.m_deltaTime = std::chrono::duration<double>(end - start).count(); // In seconds
		g_time.m_fps = 1.0f / g_time.m_deltaTime;
		start = end;

		sumDeltaTime += g_time.m_deltaTime;
		avgCount++;

		if (sumDeltaTime >= 100)
		{
			avgDeltaTime = sumDeltaTime / avgCount;
			sumDeltaTime = 0;
			avgCount = 0;
		}
	}

	return 0;
}

void Engine::Update()
{	
	if (m_scene)
	{
		// Update Camera
		if (m_renderWindowFocused)
			g_cameraManager.Update();

		// Update Scene
		m_scene->ExternalUpdate();

		// Update Physics
		if (g_engineSettings.physicsEnabled && m_scene->PhysicsEnabled())
			m_physicsEngine.Update(m_scene);
	}
}

void Engine::Render()
{
	TracyD3D11ZoneC(g_tracyContext, "Render", RandomUniqueColor());

	// Render Scene
	if (m_scene)
		m_scene->Render(g_cameraManager.GetCamera());

	// Render UI
	{
		ZoneScopedNC("ImGui", RandomUniqueColor());
		TracyD3D11ZoneC(g_tracyContext, "ImGui", RandomUniqueColor());

		ImGuiHandler::SetNewFrame();

		RenderWindowUI();

		if (g_engineSettings.imguiOn)
		{
			ImGuiHandler::InformationPanel();
			ImGuiHandler::ScreenStatistics();

			if (m_scene)
				m_scene->RenderUI();

		}

		ImGuiHandler::Render();
	}
	//else
		//DeviceManager::BackBuffer()->Copy(DeviceManager::RenderRTV());
}
