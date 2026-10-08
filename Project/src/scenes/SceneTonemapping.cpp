#include "stdafx.h"

#include "SceneTonemapping.h"

#include <iostream>
#include <ostream>
#include <string>

ID csSceneToneMapping,
   csSceneToneMappingS;

Pipeline* tonemapPipeline = nullptr;
Pipeline* tonemapPipelineS = nullptr;

struct SceneToneMappingSettings {
	int toneMap = 0;
	float gammaFactor = 2.2f;
	int padding[2];
} sceneToneMappingSettings;

SceneTonemapping::~SceneTonemapping()
{
	ZoneScopedC(RandomUniqueColor());
}

void SceneTonemapping::Load()
{
	ZoneScopedC(RandomUniqueColor());

	ID	vs = m_resourceManager.AddShader("Default_VS", "Default_VS", ShaderType::VERTEX_SHADER),
		ps = m_resourceManager.AddShader("PBR_PS", "PBR_PS", ShaderType::PIXEL_SHADER),
		cs = m_resourceManager.AddShader("Blur_CS", "Blur_CS", ShaderType::COMPUTE_SHADER),
		csSSAO = m_resourceManager.AddShader("SSAO_CS", "SSAO_CS", ShaderType::COMPUTE_SHADER),
		mesh = m_resourceManager.AddMesh("res/ObjectFiles/", "teapot.obj", "Teapot"),
		mesh2 = m_resourceManager.AddMesh("res/ObjectFiles/", "Cube.obj", "Cube"),
		sponza = m_resourceManager.AddMesh("res/ObjectFiles/", "sponza_1K_noDecals.glb", "Sponza"),
		tex = m_resourceManager.AddTexture("res/Textures/Magenta.png", "Grid"),
		tex2 = m_resourceManager.AddTexture("res/Textures/Spaceships/Imperial_Orange.png", "ImperialOrange"),
		samp = m_resourceManager.AddSampler("Sampler", D3D11_TEXTURE_ADDRESS_WRAP);

	csSceneToneMapping = m_resourceManager.AddShader("post_effects/ToneMapping_CS", "ToneMapping_CS", ShaderType::COMPUTE_SHADER);
	csSceneToneMappingS = m_resourceManager.AddShader("post_effects/ToneMapping_CS_S", "ToneMapping_CS_S", ShaderType::COMPUTE_SHADER);

	Entity* ent1 = CreateEntity();
	{
		ent1->SetMesh(mesh);
		ent1->SetSampler(samp);
		ent1->SetTransform(Transform({ -3, 4, -4 }, { 0, 0, 0 }, { 0.1f, 0.1f, 0.1f }));
		ent1->SetMaterial(MaterialMaps(tex));
	}

	Entity* ent2 = CreateEntity();
	{
		ent2->SetMesh(mesh2); 
		ent2->SetSampler(samp);
		ent2->SetTransform(Transform({ 5, 4, 4 }, { 0, 0, 0 }, { 1, 1, 1 }));
		ent2->SetMaterial(MaterialMaps(tex));
	}

	Entity* entSponza = CreateEntity();
	{
		entSponza->SetMesh(sponza); 
		entSponza->SetSampler(samp);
		entSponza->SetTransform(Transform({ 0, 0, 0 }, { 0, 0, 0 }, { 0.01, 0.01, 0.01 }));
	}

	PointLight light = PointLight({ -2, 5, 0 }, { 1, 1, 1 }, 100, 0, 8);

	// Default pipeline
	{
		tonemapPipeline = CreatePipeline();

		tonemapPipeline->SetShader(vs, ShaderType::VERTEX_SHADER);
		std::pair<const char*, DXGI_FORMAT> layout[] = {
			std::make_pair("POSITION", DXGI_FORMAT_R32G32B32_FLOAT),
			std::make_pair("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT),
			std::make_pair("UV", DXGI_FORMAT_R32G32_FLOAT),
			std::make_pair("TANGENT", DXGI_FORMAT_R32G32B32_FLOAT),
			std::make_pair("BITANGENT", DXGI_FORMAT_R32G32B32_FLOAT)
		};
		tonemapPipeline->AddInputLayout(layout, 5, m_resourceManager);

		tonemapPipeline->SetShader(ps, ShaderType::PIXEL_SHADER);
		DX::XMFLOAT4X4 w, vp;
		DX::XMStoreFloat4x4(&w, DX::XMMatrixIdentity());
		DX::XMStoreFloat4x4(&vp, DX::XMMatrixIdentity());
		DX::XMFLOAT4X4 matrices[2] = { w, vp };
		tonemapPipeline->AddConstantBuffer(0, matrices, 2 * sizeof(DX::XMFLOAT4X4), vs, m_resourceManager);

		tonemapPipeline->SetShader(csSceneToneMapping, ShaderType::COMPUTE_SHADER);
		tonemapPipeline->AddConstantBuffer(0, (void*)&sceneToneMappingSettings, sizeof(SceneToneMappingSettings), csSceneToneMapping, m_resourceManager);

		//pipeline->SetShader(csAA, ShaderType::COMPUTE_SHADER);

		tonemapPipeline->SetRasterizer(new RasterizerD3D11());

		tonemapPipeline->AddEntity(entSponza);

		tonemapPipeline->SetGlobalLighAmbiance(0.01f);
		tonemapPipeline->AddLight(light);

		m_renderer.AddPipeline(tonemapPipeline);
	}

	// Specialized pipeline
	{
		tonemapPipelineS = CreatePipeline();

		tonemapPipelineS->SetShader(vs, ShaderType::VERTEX_SHADER);
		std::pair<const char*, DXGI_FORMAT> layout[] = {
			std::make_pair("POSITION", DXGI_FORMAT_R32G32B32_FLOAT),
			std::make_pair("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT),
			std::make_pair("UV", DXGI_FORMAT_R32G32_FLOAT),
			std::make_pair("TANGENT", DXGI_FORMAT_R32G32B32_FLOAT),
			std::make_pair("BITANGENT", DXGI_FORMAT_R32G32B32_FLOAT)
		};
		tonemapPipelineS->AddInputLayout(layout, 5, m_resourceManager);

		tonemapPipelineS->SetShader(ps, ShaderType::PIXEL_SHADER);
		DX::XMFLOAT4X4 w, vp;
		DX::XMStoreFloat4x4(&w, DX::XMMatrixIdentity());
		DX::XMStoreFloat4x4(&vp, DX::XMMatrixIdentity());
		DX::XMFLOAT4X4 matrices[2] = { w, vp };
		tonemapPipelineS->AddConstantBuffer(0, matrices, 2 * sizeof(DX::XMFLOAT4X4), vs, m_resourceManager);

		tonemapPipelineS->SetShader(csSceneToneMapping, ShaderType::COMPUTE_SHADER);
		tonemapPipelineS->AddConstantBuffer(0, (void*)&sceneToneMappingSettings, sizeof(SceneToneMappingSettings), csSceneToneMapping, m_resourceManager);

		//pipeline->SetShader(csAA, ShaderType::COMPUTE_SHADER);

		tonemapPipelineS->SetRasterizer(new RasterizerD3D11());

		tonemapPipelineS->AddEntity(entSponza);

		tonemapPipelineS->SetGlobalLighAmbiance(0.01f);
		tonemapPipelineS->AddLight(light);

		m_renderer.AddPipeline(tonemapPipelineS);
	}

}

void SceneTonemapping::Unload()
{
}

void SceneTonemapping::Update()
{
	ZoneScopedC(RandomUniqueColor());

	tonemapPipeline->UpdateConstantBuffer(0, (void*)&sceneToneMappingSettings, csSceneToneMapping, m_resourceManager);
}

void SceneTonemapping::InternalRenderUI()
{
	ZoneScopedC(RandomUniqueColor());

	ImGui::Begin("Settings");

	ImGui::SeparatorText("Tone Mapping");

	if (ImGui::InputInt("Tone Map", &sceneToneMappingSettings.toneMap, 1, 1))
		sceneToneMappingSettings.toneMap = std::clamp(sceneToneMappingSettings.toneMap, 0, 3);

	if (ImGui::InputFloat("Gamme Factor", &sceneToneMappingSettings.gammaFactor, 0.1f, 0.2f))
		sceneToneMappingSettings.gammaFactor = std::clamp<float>(sceneToneMappingSettings.gammaFactor, 0.2f, 4.4f);

	ImGui::End();
}
