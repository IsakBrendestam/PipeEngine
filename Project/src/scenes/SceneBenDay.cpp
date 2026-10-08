#include "stdafx.h"

#include "SceneBenDay.h"

struct SettingsBenDay
{
	float resolution[2];
	float dotSpacing;
	float dotScale;
	float angles[3];
	float temp; //Padding
} settingsBenDay;

SceneBenDay::~SceneBenDay()
{
	ZoneScopedC(RandomUniqueColor());
}

void SceneBenDay::Load()
{
	ZoneScopedC(RandomUniqueColor());

	settingsBenDay.resolution[0] = g_engineSettings.renderResolution[0];
	settingsBenDay.resolution[1] = g_engineSettings.renderResolution[1];

	settingsBenDay.dotSpacing = 3.66f;
	settingsBenDay.dotScale = 1.095f;

	settingsBenDay.angles[0] = 0.26f;
	settingsBenDay.angles[1] = 0.11f;
	settingsBenDay.angles[2] = 0.16f;

	ID vs = m_resourceManager.AddShader("Color_VS", "Color_VS", ShaderType::VERTEX_SHADER),
		ps = m_resourceManager.AddShader("Color_PS", "Color_PS", ShaderType::PIXEL_SHADER),
		mesh = m_resourceManager.AddMesh("res/ObjectFiles/", "teapot.obj", "Teapot"),
		mesh2 = m_resourceManager.AddMesh("res/ObjectFiles/", "Cube.obj", "Cube"),
		tex = m_resourceManager.AddTexture("res/Textures/LightBlue.png", "LightBlue"),
		tex2 = m_resourceManager.AddTexture("res/Textures/Gray.png", "Gray"),
		samp = m_resourceManager.AddSampler("Sampler", D3D11_TEXTURE_ADDRESS_WRAP);

	m_cs = m_resourceManager.AddShader("BenDayFilter_CS", "BenDayFilter_CS", ShaderType::COMPUTE_SHADER);
	
	m_resourceManager.AddShader("PostEffectCTR_CS", "PostEffectCTR_CS", ShaderType::COMPUTE_SHADER);
	m_resourceManager.AddShader("Default_CS", "Default_CS", ShaderType::COMPUTE_SHADER);

	{
		vs = m_resourceManager.AddShader("Texture_VS", "Texture_VS", ShaderType::VERTEX_SHADER);
		ps = m_resourceManager.AddShader("Texture_PS", "Texture_PS", ShaderType::PIXEL_SHADER);

		Pipeline* pipeline = CreatePipeline();

		pipeline->SetShader(vs, ShaderType::VERTEX_SHADER);
		std::pair<const char*, DXGI_FORMAT> layout[] = {
			std::make_pair("POSITION", DXGI_FORMAT_R32G32B32_FLOAT),
			std::make_pair("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT),
			std::make_pair("UV", DXGI_FORMAT_R32G32_FLOAT)
		};
		pipeline->AddInputLayout(layout, 3, m_resourceManager);

		pipeline->SetShader(ps, ShaderType::PIXEL_SHADER);
		DX::XMFLOAT4X4 w, vp;
		DX::XMStoreFloat4x4(&w, DX::XMMatrixIdentity());
		DX::XMStoreFloat4x4(&vp, DX::XMMatrixIdentity());
		DX::XMFLOAT4X4 matrices[2] = { w, vp };
		pipeline->AddConstantBuffer(0, matrices, 2 * sizeof(DX::XMFLOAT4X4), vs, m_resourceManager);

		pipeline->AddLight(PointLight({ 0, 10, 0 }, { 1, 1, 1 }, 150, 1, 8));

		pipeline->SetShader(m_cs, ShaderType::COMPUTE_SHADER);
		pipeline->AddConstantBuffer(0, &settingsBenDay, sizeof(SettingsBenDay), m_cs, m_resourceManager);


		pipeline->SetRasterizer(new RasterizerD3D11());

		{
			Entity* ent = CreateEntity();
			ent->SetMesh(mesh);
			ent->SetSampler(samp);
			ent->SetTransform(Transform({ -3, 4, -4 }, { 0, 0, 0 }, { 0.1f, 0.1f, 0.1f }));
			ent->SetMaterial(MaterialMaps(tex));
			pipeline->AddEntity(ent);
		}

		{
			Entity* ent = CreateEntity();
			ent->SetMesh(mesh2);
			ent->SetSampler(samp);
			ent->SetTransform(Transform({ 5, 4, 4 }, { 0, 0, 0 }, { 1, 1, 1 }));
			ent->SetMaterial(MaterialMaps(tex));

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

		/*
		// Room
		{
			mesh = m_resourceManager.AddMesh("res/ObjectFiles/", "Gallery.obj", "Gallery");
			tex = m_resourceManager.AddTexture("res/Textures/Gallery.jpg", "Gallery");

			Entity* ent = CreateEntity();
			ent->SetMesh(mesh);
			ent->SetSampler(samp);
			ent->SetTransform(Transform({ 0, 0, 0 }, { 0, 0, 0 }, { 10, 10, 10 }));
			ent->SetMaterial(MaterialMaps(tex));

			pipeline->AddEntity(ent);
		}
		*/

		m_renderer.AddPipeline(pipeline);
	}
}

void SceneBenDay::Unload()
{
}

void SceneBenDay::Update()
{
	ZoneScopedC(RandomUniqueColor());

	GetPipeline(0)->UpdateConstantBuffer(0, &settingsBenDay, m_cs, m_resourceManager);
}

void SceneBenDay::InternalRenderUI()
{
	ZoneScopedC(RandomUniqueColor());

	ImGui::Begin("Settings");

	ImGui::DragFloat2("Resolution", settingsBenDay.resolution, 0.1f, 0.0f, 1920);
	ImGui::DragFloat("Dot Scale", &settingsBenDay.dotScale, 0.001f, 0.0f, 10);
	ImGui::DragFloat("Dot Spacing", &settingsBenDay.dotSpacing, 0.01f, 0.0f, 50);
	ImGui::DragFloat4("Rotation", settingsBenDay.angles, 0.01f, 0.0f, 360);

	ImGui::End();
}
