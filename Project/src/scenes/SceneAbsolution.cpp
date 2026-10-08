#include "stdafx.h"
#include "SceneAbsolution.h"

struct SettingsAbsolution
{
	double time;
	float noiseScale[2] { 2.25f, 2.25f };
} settingsAbsolution;


SceneAbsolution::~SceneAbsolution()
{
	ZoneScopedC(RandomUniqueColor());
}

void SceneAbsolution::Load()
{
	ZoneScopedC(RandomUniqueColor());

	ID  vs = m_resourceManager.AddShader("Color_VS", "Color_VS", ShaderType::VERTEX_SHADER),
		ps = m_resourceManager.AddShader("Color_PS", "Color_PS", ShaderType::PIXEL_SHADER),
		cs = m_resourceManager.AddShader("Absolution_CS", "Absolution_CS", ShaderType::COMPUTE_SHADER),

		meshFreezer = m_resourceManager.AddMesh("res/ObjectFiles/", "Freezer.obj", "Freezer"),
		mesh2 = m_resourceManager.AddMesh("res/ObjectFiles/", "Cube.obj", "Cube"),

		meshWalls = m_resourceManager.AddMesh("res/ObjectFiles/VentilationRoom/", "Walls.obj", "Walls"),
		meshFloor = m_resourceManager.AddMesh("res/ObjectFiles/VentilationRoom/", "Floor.obj", "Floor"),
		meshBaseBoard = m_resourceManager.AddMesh("res/ObjectFiles/VentilationRoom/", "BaseBoard.obj", "BaseBoard"),
		meshVentilation = m_resourceManager.AddMesh("res/ObjectFiles/VentilationRoom/", "Ventilation.obj", "Ventilation"),
		meshVentilationFrame = m_resourceManager.AddMesh("res/ObjectFiles/VentilationRoom/", "VentilationFrame.obj", "VentilationFrame"),

		texWallsBase = m_resourceManager.AddTexture("res/Textures/VentilationRoom/Wall_Base_color_1001.png", "WallsBase", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB),
		texFloorBase = m_resourceManager.AddTexture("res/Textures/VentilationRoom/Floor_Base_color_1001.png", "FloorBase", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB),
		texBaseBoardBase = m_resourceManager.AddTexture("res/Textures/VentilationRoom/BaseBoard_Base_color_1001.png", "BaseBoardBase", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB),
		texVentilationBase = m_resourceManager.AddTexture("res/Textures/VentilationRoom/Ventilation_Base_color_1001.png", "VentilationBase", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB),
		texVentilationFrameBase = m_resourceManager.AddTexture("res/Textures/VentilationRoom/VentilationFrame_Base_color_1001.png", "VentilationFrameBase", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB),

		texWallsNormal = m_resourceManager.AddTexture("res/Textures/VentilationRoom/Wall_Normal_DirectX_1001.png", "WallsNormal"),
		texFloorNormal = m_resourceManager.AddTexture("res/Textures/VentilationRoom/Floor_Normal_DirectX_1001.png", "FloorNormal"),
		texBaseBoardNormal = m_resourceManager.AddTexture("res/Textures/VentilationRoom/BaseBoard_Normal_DirectX_1001.png", "BaseBoardNormal"),
		texVentilationNormal = m_resourceManager.AddTexture("res/Textures/VentilationRoom/Ventilation_Normal_DirectX_1001.png", "VentilationNormal"),
		texVentilationFrameNormal = m_resourceManager.AddTexture("res/Textures/VentilationRoom/VentilationFrame_Normal_DirectX_1001.png", "VentilationFrameNormal"),

		texWallsAO = m_resourceManager.AddTexture("res/Textures/VentilationRoom/Wall_Mixed_AO_1001.png", "WallsAO", DXGI_FORMAT_R8G8B8A8_UNORM),
		texFloorAO = m_resourceManager.AddTexture("res/Textures/VentilationRoom/Floor_Mixed_AO_1001.png", "FloorAO", DXGI_FORMAT_R8G8B8A8_UNORM),
		texBaseBoardAO = m_resourceManager.AddTexture("res/Textures/VentilationRoom/BaseBoard_Mixed_AO_1001.png", "BaseBoardAO", DXGI_FORMAT_R8G8B8A8_UNORM),
		texVentilationAO = m_resourceManager.AddTexture("res/Textures/VentilationRoom/Ventilation_Mixed_AO_1001.png", "VentilationAO", DXGI_FORMAT_R8G8B8A8_UNORM),
		texVentilationFrameAO = m_resourceManager.AddTexture("res/Textures/VentilationRoom/VentilationFrame_Mixed_AO_1001.png", "VentilationFrameAO", DXGI_FORMAT_R8G8B8A8_UNORM),

		tex = m_resourceManager.AddTexture("res/Textures/LightBlue.png", "LightBlue", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB),
		tex2 = m_resourceManager.AddTexture("res/Textures/Gray.png", "Gray", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB),
		tex3 = m_resourceManager.AddTexture("res/Textures/DarkGray.png", "DarkGray", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB),
		tex4 = m_resourceManager.AddTexture("res/Textures/Freezer/Body_Base_color_1001.png", "FreezerBody", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB),

		samp = m_resourceManager.AddSampler("Sampler", D3D11_TEXTURE_ADDRESS_WRAP);
	
	m_resourceManager.AddShader("Default_CS", "Default_CS", ShaderType::COMPUTE_SHADER);

	Entity* walls = CreateEntity();
	Entity* floor = CreateEntity();
	Entity* baseBoard = CreateEntity();
	Entity* ventelation = CreateEntity();
	Entity* ventelationFrame = CreateEntity();
	{
		Transform t = Transform({ -3, 4, -4 }, { 0, 0, 0 }, { 0.5f, 0.5f, 0.5f });

		walls->SetMesh(meshWalls);
		walls->SetSampler(samp);
		walls->SetTransform(t);
		walls->SetMaterial({ texWallsBase, texWallsNormal, texWallsAO });

		floor->SetMesh(meshFloor);
		floor->SetSampler(samp);
		floor->SetTransform(t);
		floor->SetMaterial({ texFloorBase, texFloorNormal, texFloorAO });

		baseBoard->SetMesh(meshBaseBoard);
		baseBoard->SetSampler(samp);
		baseBoard->SetTransform(t);
		baseBoard->SetMaterial({ texBaseBoardBase, texBaseBoardNormal, texBaseBoardAO });

		ventelation->SetMesh(meshVentilation);
		ventelation->SetSampler(samp);
		ventelation->SetTransform(t);
		ventelation->SetMaterial({ texVentilationBase, texVentilationNormal, texVentilationAO });

		ventelationFrame->SetMesh(meshVentilationFrame);
		ventelationFrame->SetSampler(samp);
		ventelationFrame->SetTransform(t);
		ventelationFrame->SetMaterial({ texVentilationFrameBase, texVentilationFrameNormal, texVentilationFrameAO });
	}

	// Normal pipeline
	{
		vs = m_resourceManager.AddShader("Default_VS", "Default_VS", ShaderType::VERTEX_SHADER);
		ps = m_resourceManager.AddShader("Default_PS", "Default_PS", ShaderType::PIXEL_SHADER);

		Pipeline* pipeline = CreatePipeline();

		pipeline->SetShader(vs, ShaderType::VERTEX_SHADER);
		std::pair<const char*, DXGI_FORMAT> layout[] = {
			std::make_pair("POSITION", DXGI_FORMAT_R32G32B32_FLOAT),
			std::make_pair("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT),
			std::make_pair("UV", DXGI_FORMAT_R32G32_FLOAT),
			std::make_pair("TANGENT", DXGI_FORMAT_R32G32B32_FLOAT),
			std::make_pair("BITANGENT", DXGI_FORMAT_R32G32B32_FLOAT)
		};
		pipeline->AddInputLayout(layout, 5, m_resourceManager);

		pipeline->SetShader(ps, ShaderType::PIXEL_SHADER);
		DX::XMFLOAT4X4 w, vp;
		DX::XMStoreFloat4x4(&w, DX::XMMatrixIdentity());
		DX::XMStoreFloat4x4(&vp, DX::XMMatrixIdentity());
		DX::XMFLOAT4X4 matrices[2] = { w, vp };
		pipeline->AddConstantBuffer(0, matrices, 2 * sizeof(DX::XMFLOAT4X4), vs, m_resourceManager);

		pipeline->AddLight(PointLight({ 4, 9, -14 }, { 1, 1, 1 }, 150, 1, 8));

		pipeline->SetRasterizer(new RasterizerD3D11());

		// Ventelation Room
		pipeline->AddEntity(walls);
		pipeline->AddEntity(floor);
		pipeline->AddEntity(baseBoard);
		pipeline->AddEntity(ventelation);
		pipeline->AddEntity(ventelationFrame);

		// Wall
		{
			Entity* ent = CreateEntity();
			ent->SetMesh(mesh2);
			ent->SetSampler(samp);
			ent->SetTransform(Transform({ 5, 4, 4 }, { 0, 0, 0 }, { 1, 1, 1 }));
			ent->SetMaterial(MaterialMaps(tex3));

			pipeline->AddEntity(ent);
		}

		// Room
		ID tempMesh = m_resourceManager.AddMesh("res/ObjectFiles/", "InvertedCube.obj", "InvertedCube");
		{
			Entity* ent = CreateEntity();
			ent->SetMesh(tempMesh);
			ent->SetSampler(samp);
			ent->SetTransform(Transform({ 0, 10, 0 }, { 0, 0, 0 }, { 30, 20, 30 }));
			ent->SetMaterial(MaterialMaps(tex2));

			pipeline->AddEntity(ent);
		}

		m_renderer.AddPipeline(pipeline);
	}

	// Absolution-Instinct Filter
	{
		vs = m_resourceManager.AddShader("Texture_VS", "Texture_VS", ShaderType::VERTEX_SHADER);
		m_ps = m_resourceManager.AddShader("Absolution_PS", "Absolution_PS", ShaderType::PIXEL_SHADER);

		Pipeline* pipeline = CreatePipeline();

		pipeline->SetClearColor({ 0, 0, 0, 1 });
		pipeline->ClearRenderTarget(false);

		pipeline->SetShader(vs, ShaderType::VERTEX_SHADER);
		std::pair<const char*, DXGI_FORMAT> layout[] = {
			std::make_pair("POSITION", DXGI_FORMAT_R32G32B32_FLOAT),
			std::make_pair("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT),
			std::make_pair("UV", DXGI_FORMAT_R32G32_FLOAT)
		};
		pipeline->AddInputLayout(layout, 3, m_resourceManager);

		DX::XMFLOAT4X4 w, vp;
		DX::XMStoreFloat4x4(&w, DX::XMMatrixIdentity());
		DX::XMStoreFloat4x4(&vp, DX::XMMatrixIdentity());
		DX::XMFLOAT4X4 matrices[2] = { w, vp };
		pipeline->AddConstantBuffer(0, matrices, 2 * sizeof(DX::XMFLOAT4X4), vs, m_resourceManager);

		pipeline->SetShader(m_ps, ShaderType::PIXEL_SHADER);
		pipeline->AddConstantBuffer(2, &settingsAbsolution, sizeof(SettingsAbsolution), m_ps, m_resourceManager);

		pipeline->SetShader(cs, ShaderType::COMPUTE_SHADER);

		pipeline->SetRasterizer(new RasterizerD3D11());

		// Highlighted object
		pipeline->AddEntity(ventelation);

		m_renderer.AddPipeline(pipeline);
	}
}

void SceneAbsolution::Unload()
{
}

void SceneAbsolution::Update()
{
	ZoneScopedC(RandomUniqueColor());

	settingsAbsolution.time += g_time.DeltaTime<double>();
	GetPipeline(1)->UpdateConstantBuffer(2, &settingsAbsolution, m_ps, m_resourceManager);
}

void SceneAbsolution::InternalRenderUI()
{
	ZoneScopedC(RandomUniqueColor());

	ImGui::Begin("Settings");

	static bool uniform = true;
	if (ImGui::DragFloat2("Resolution", settingsAbsolution.noiseScale, 0.1f, -10.0f, 1000) && uniform)
		settingsAbsolution.noiseScale[1] = settingsAbsolution.noiseScale[0];
	ImGui::SameLine();
	ImGui::Checkbox("Uniform", &uniform);

	ImGui::End();
}
