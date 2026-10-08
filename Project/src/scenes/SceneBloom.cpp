#include "stdafx.h"

#include "SceneBloom.h"

#include <iostream>
#include <ostream>
#include <string>

ID csToneMappingBloom,
   csBright,
   csBloom;

Pipeline* bloomPipeline = nullptr;

constexpr char sampleCount = 4;
RenderTargetD3D11* brightness;
std::array<RenderTargetD3D11*, sampleCount> downsamples;
std::array<RenderTargetD3D11*, sampleCount> upsamples;

struct BloomToneMappingSettings {
	int toneMap = 0;
	float gammaFactor = 2.2f;
	int padding[2];
} bloomToneMappingSettings;

struct BloomSettings {
	float threshols = 0.7f;
	float strength = 1.0f;
	int padding[2];
} bloomSettings;

SceneBloom::~SceneBloom()
{
	ZoneScopedC(RandomUniqueColor());

	delete brightness;
	for (int i = 0; i < sampleCount; i++)
	{
		delete downsamples[i];
		delete upsamples[i];
	}
}

void SceneBloom::Load()
{
	ZoneScopedC(RandomUniqueColor());

	ID	vs = m_resourceManager.AddShader("Default_VS", "Default_VS", ShaderType::VERTEX_SHADER),
		ps = m_resourceManager.AddShader("PBR_PS", "PBR_PS", ShaderType::PIXEL_SHADER),
		cs = m_resourceManager.AddShader("Blur_CS", "Blur_CS", ShaderType::COMPUTE_SHADER),
		csSSAO = m_resourceManager.AddShader("SSAO_CS", "SSAO_CS", ShaderType::COMPUTE_SHADER),
		csDown = m_resourceManager.AddShader("post_effects/Downsample_CS", "Downsample_CS", ShaderType::COMPUTE_SHADER),
		csUp = m_resourceManager.AddShader("post_effects/Upsample_CS", "Upsample_CS", ShaderType::COMPUTE_SHADER),
		mesh = m_resourceManager.AddMesh("res/ObjectFiles/", "teapot.obj", "Teapot"),
		mesh2 = m_resourceManager.AddMesh("res/ObjectFiles/", "Cube.obj", "Cube"),
		sponza = m_resourceManager.AddMesh("res/ObjectFiles/", "sponza_1K_noDecals.glb", "Sponza"),
		tex = m_resourceManager.AddTexture("res/Textures/Magenta.png", "Grid"),
		tex2 = m_resourceManager.AddTexture("res/Textures/Spaceships/Imperial_Orange.png", "ImperialOrange"),
		samp = m_resourceManager.AddSampler("Sampler", D3D11_TEXTURE_ADDRESS_WRAP);

	csToneMappingBloom = m_resourceManager.AddShader("post_effects/ToneMapping_CS", "ToneMapping_CS", ShaderType::COMPUTE_SHADER);
	csBright = m_resourceManager.AddShader("post_effects/BrightExtraction_CS", "BrightExtraction_CS", ShaderType::COMPUTE_SHADER);
	csBloom = m_resourceManager.AddShader("post_effects/Bloom_CS", "Bloom_CS", ShaderType::COMPUTE_SHADER);

	brightness = new RenderTargetD3D11(g_engineSettings.renderResolution[0], g_engineSettings.renderResolution[1], DXGI_FORMAT_R16G16B16A16_FLOAT);

	for (int i = 0; i < sampleCount; i++)
		downsamples[i] = new RenderTargetD3D11(g_engineSettings.renderResolution[0] / std::pow(2, i+1), g_engineSettings.renderResolution[1] / std::pow(2, i+1), DXGI_FORMAT_R16G16B16A16_FLOAT);

	for (int i = 0; i < sampleCount; i++)
		upsamples[sampleCount - (i+1)] = new RenderTargetD3D11(g_engineSettings.renderResolution[0] / std::pow(2, i+1), g_engineSettings.renderResolution[1] / std::pow(2, i+1), DXGI_FORMAT_R16G16B16A16_FLOAT);

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

	// Default Pipeline
	{
		bloomPipeline = CreatePipeline();

		bloomPipeline->SetShader(vs, ShaderType::VERTEX_SHADER);
		std::pair<const char*, DXGI_FORMAT> layout[] = {
			std::make_pair("POSITION", DXGI_FORMAT_R32G32B32_FLOAT),
			std::make_pair("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT),
			std::make_pair("UV", DXGI_FORMAT_R32G32_FLOAT),
			std::make_pair("TANGENT", DXGI_FORMAT_R32G32B32_FLOAT),
			std::make_pair("BITANGENT", DXGI_FORMAT_R32G32B32_FLOAT)
		};
		bloomPipeline->AddInputLayout(layout, 5, m_resourceManager);

		bloomPipeline->SetShader(ps, ShaderType::PIXEL_SHADER);
		DX::XMFLOAT4X4 w, vp;
		DX::XMStoreFloat4x4(&w, DX::XMMatrixIdentity());
		DX::XMStoreFloat4x4(&vp, DX::XMMatrixIdentity());
		DX::XMFLOAT4X4 matrices[2] = { w, vp };
		bloomPipeline->AddConstantBuffer(0, matrices, 2 * sizeof(DX::XMFLOAT4X4), vs, m_resourceManager);

		int shaderIndex = 0;

		// Brightness Extraction
		bloomPipeline->SetShader(csBright, ShaderType::COMPUTE_SHADER);
		bloomPipeline->SetOutTexture(brightness->GetUAV(), ShaderType::COMPUTE_SHADER, 0, shaderIndex);
		bloomPipeline->AddConstantBuffer(0, (void*)&bloomSettings, sizeof(BloomSettings), csBright, m_resourceManager);
		shaderIndex++;

		// Downsample
		for (char i = 0; i < sampleCount; i++)
		{
			bloomPipeline->SetShader(csDown, ShaderType::COMPUTE_SHADER);

			if (i != 0)
				bloomPipeline->AddTexture(downsamples[i-1]->GetSRV(), ShaderType::COMPUTE_SHADER, 0, shaderIndex);
			else
				bloomPipeline->AddTexture(brightness->GetSRV(), ShaderType::COMPUTE_SHADER, 0, shaderIndex);

			bloomPipeline->SetOutTexture(downsamples[i]->GetUAV(), ShaderType::COMPUTE_SHADER, 0, shaderIndex);
			shaderIndex++;
		}

		// Upsample
		for (char i = 0; i < sampleCount; i++)
		{
			bloomPipeline->SetShader(csUp, ShaderType::COMPUTE_SHADER);

			// High res
			bloomPipeline->AddTexture(downsamples[sampleCount-1-i]->GetSRV(), ShaderType::COMPUTE_SHADER, 0, shaderIndex);

			// Low res
			if (i == 0)
				bloomPipeline->AddTexture(downsamples[sampleCount-1]->GetSRV(), ShaderType::COMPUTE_SHADER, 1, shaderIndex);
			else
				bloomPipeline->AddTexture(upsamples[i-1]->GetSRV(), ShaderType::COMPUTE_SHADER, 1, shaderIndex);

			bloomPipeline->SetOutTexture(upsamples[i]->GetUAV(), ShaderType::COMPUTE_SHADER, 0, shaderIndex);

			shaderIndex++;
		}

		// Bloom
		bloomPipeline->SetShader(csBloom, ShaderType::COMPUTE_SHADER);
		bloomPipeline->AddTexture(upsamples[sampleCount-1]->GetSRV(), ShaderType::COMPUTE_SHADER, 1, shaderIndex);
		bloomPipeline->AddConstantBuffer(0, (void*)&bloomSettings, sizeof(BloomSettings), csBloom, m_resourceManager);
		shaderIndex++;

		// Tone mapping
		bloomPipeline->SetShader(csToneMappingBloom, ShaderType::COMPUTE_SHADER);
		bloomPipeline->AddConstantBuffer(0, (void*)&bloomToneMappingSettings, sizeof(BloomToneMappingSettings), csToneMappingBloom, m_resourceManager);
		shaderIndex++;

		bloomPipeline->SetRasterizer(new RasterizerD3D11());

		bloomPipeline->AddEntity(entSponza);

		bloomPipeline->SetGlobalLighAmbiance(0.01f);
		bloomPipeline->AddLight(light);

		m_renderer.AddPipeline(bloomPipeline);
	}
}

void SceneBloom::Unload()
{
}

void SceneBloom::Update()
{
	ZoneScopedC(RandomUniqueColor());

	bloomPipeline->UpdateConstantBuffer(0, (void*)&bloomToneMappingSettings, csToneMappingBloom, m_resourceManager);
	bloomPipeline->UpdateConstantBuffer(0, (void*)&bloomSettings, csBright, m_resourceManager);
	bloomPipeline->UpdateConstantBuffer(0, (void*)&bloomSettings, csBloom, m_resourceManager);
}

void SceneBloom::InternalRenderUI()
{
	ZoneScopedC(RandomUniqueColor());

	ImGui::Begin("Settings");

	ImGui::SeparatorText("Bloom");
	ImGui::DragFloat("Threshold", &bloomSettings.threshols, 0.01f, 0.0f, 1.0f);
	ImGui::DragFloat("Strength", &bloomSettings.strength, 0.1f, 0.0f, 10.0f);
		

	ImGui::SeparatorText("Tone Mapping");

	if (ImGui::InputInt("Tone Map", &bloomToneMappingSettings.toneMap, 1, 1))
		bloomToneMappingSettings.toneMap = std::clamp(bloomToneMappingSettings.toneMap, 0, 3);

	if (ImGui::InputFloat("Gamma Factor", &bloomToneMappingSettings.gammaFactor, 0.1f, 0.2f))
		bloomToneMappingSettings.gammaFactor = std::clamp<float>(bloomToneMappingSettings.gammaFactor, 0.2f, 4.4f);

	ImGui::End();
}
