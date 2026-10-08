#include "stdafx.h"
#include "SceneStressTest.h"

#include "math/Random.h"

SceneStressTest::~SceneStressTest()
{
}


void SceneStressTest::Load()
{
	ID  vs = m_resourceManager.AddShader("Color_VS", "Color_VS", ShaderType::VERTEX_SHADER),
		ps = m_resourceManager.AddShader("Color_PS", "Color_PS", ShaderType::PIXEL_SHADER),
		cs = m_resourceManager.AddShader("Absolution_CS", "Absolution_CS", ShaderType::COMPUTE_SHADER);

	meshes = {
		m_resourceManager.AddMesh("res/ObjectFiles/", "Freezer.obj", "Freezer"),
		m_resourceManager.AddMesh("res/ObjectFiles/", "Cube.obj", "Cube"),
		m_resourceManager.AddMesh("res/ObjectFiles/VentilationRoom/", "Walls.obj", "Walls"),
		m_resourceManager.AddMesh("res/ObjectFiles/VentilationRoom/", "Floor.obj", "Floor"),
		m_resourceManager.AddMesh("res/ObjectFiles/VentilationRoom/", "BaseBoard.obj", "BaseBoard"),
		m_resourceManager.AddMesh("res/ObjectFiles/VentilationRoom/", "Ventilation.obj", "Ventilation"),
		m_resourceManager.AddMesh("res/ObjectFiles/VentilationRoom/", "VentilationFrame.obj", "VentilationFrame"),
	};

	textures = {
		m_resourceManager.AddTexture("res/Textures/VentilationRoom/Wall_Base_color_1001.png", "WallsBase", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB),
		m_resourceManager.AddTexture("res/Textures/VentilationRoom/Floor_Base_color_1001.png", "FloorBase", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB),
		m_resourceManager.AddTexture("res/Textures/VentilationRoom/BaseBoard_Base_color_1001.png", "BaseBoardBase", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB),
		m_resourceManager.AddTexture("res/Textures/VentilationRoom/Ventilation_Base_color_1001.png", "VentilationBase", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB),
		m_resourceManager.AddTexture("res/Textures/VentilationRoom/VentilationFrame_Base_color_1001.png", "VentilationFrameBase", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB),

		m_resourceManager.AddTexture("res/Textures/LightBlue.png", "LightBlue", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB),
		m_resourceManager.AddTexture("res/Textures/Gray.png", "Gray", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB),
		m_resourceManager.AddTexture("res/Textures/DarkGray.png", "DarkGray", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB),
		m_resourceManager.AddTexture("res/Textures/Freezer/Body_Base_color_1001.png", "FreezerBody", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB),
	};

	normals = {
		m_resourceManager.AddTexture("res/Textures/VentilationRoom/Wall_Normal_DirectX_1001.png", "WallsNormal"),
		m_resourceManager.AddTexture("res/Textures/VentilationRoom/Floor_Normal_DirectX_1001.png", "FloorNormal"),
		m_resourceManager.AddTexture("res/Textures/VentilationRoom/BaseBoard_Normal_DirectX_1001.png", "BaseBoardNormal"),
		m_resourceManager.AddTexture("res/Textures/VentilationRoom/Ventilation_Normal_DirectX_1001.png", "VentilationNormal"),
		m_resourceManager.AddTexture("res/Textures/VentilationRoom/VentilationFrame_Normal_DirectX_1001.png", "VentilationFrameNormal"),
	};

	aos = {
		m_resourceManager.AddTexture("res/Textures/VentilationRoom/Wall_Mixed_AO_1001.png", "WallsAO", DXGI_FORMAT_R8G8B8A8_UNORM),
		m_resourceManager.AddTexture("res/Textures/VentilationRoom/Floor_Mixed_AO_1001.png", "FloorAO", DXGI_FORMAT_R8G8B8A8_UNORM),
		m_resourceManager.AddTexture("res/Textures/VentilationRoom/BaseBoard_Mixed_AO_1001.png", "BaseBoardAO", DXGI_FORMAT_R8G8B8A8_UNORM),
		m_resourceManager.AddTexture("res/Textures/VentilationRoom/Ventilation_Mixed_AO_1001.png", "VentilationAO", DXGI_FORMAT_R8G8B8A8_UNORM),
		m_resourceManager.AddTexture("res/Textures/VentilationRoom/VentilationFrame_Mixed_AO_1001.png", "VentilationFrameAO", DXGI_FORMAT_R8G8B8A8_UNORM),
	};

	samplers = {
		m_resourceManager.AddSampler("Sampler", D3D11_TEXTURE_ADDRESS_WRAP)
	};

	m_resourceManager.AddShader("PostEffectCTR_CS", "PostEffectCTR_CS", ShaderType::COMPUTE_SHADER);
	m_resourceManager.AddShader("Default_CS", "Default_CS", ShaderType::COMPUTE_SHADER);


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

		// Entities
		for (int i = 0; i < 1000; i++)
			AddRandomEntity(pipeline);

		m_renderer.AddPipeline(pipeline);
	}
}

void SceneStressTest::Unload()
{
}

void SceneStressTest::Update()
{
}

void SceneStressTest::InternalRenderUI()
{
	ZoneScopedC(RandomUniqueColor());

	ImGui::Begin("Settings");

	if (ImGui::Button("Add entities (200)"))
	{
		Pipeline* p = GetPipeline(0);
		for (int i = 0; i < 200; i++)
			AddRandomEntity(p);
	}
	
	ImGui::End();
}

void SceneStressTest::AddRandomEntity(Pipeline* pipeline)
{	
	Entity* e = CreateEntity();
	Transform t = Transform({ (float)m_random.Range(-100, 100), (float)m_random.Range(-100, 100), (float)m_random.Range(-100, 100) }, 
							{ (float)m_random.Range(-100, 100), (float)m_random.Range(-100, 100), (float)m_random.Range(-100, 100) }, 
							{ (float)m_random.Range(1, 300)/100.0f, (float)m_random.Range(1, 300)/100.0f, (float)m_random.Range(1, 300)/100.0f });

	e->SetMesh(meshes[m_random.Range(0, meshes.size()-1)]);
	e->SetSampler(samplers[m_random.Range(0, samplers.size()-1)]);
	e->SetTransform(t);
	e->SetMaterial({textures[m_random.Range(0, textures.size()-1)],
						normals[m_random.Range(0, normals.size()-1)],
						aos[m_random.Range(0, aos.size()-1)]});

	pipeline->AddEntity(e);

}
