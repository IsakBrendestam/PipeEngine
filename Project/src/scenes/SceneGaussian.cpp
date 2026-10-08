#include "stdafx.h"

#include "SceneGaussian.h"

#include <iostream>
#include <ostream>
#include <string>

struct GaussianSettings {
	int direction = 0;
	int radius = 3;
	int padding[2];
} gaussianSettings;

Pipeline* defaultPipeline;
Pipeline* specializedPipeline;

ID csGaussianV;
ID csGaussianH;
ID csGaussianV_S;
ID csGaussianH_S;

SceneGaussian::~SceneGaussian()
{
	ZoneScopedC(RandomUniqueColor());
}

void SceneGaussian::Load()
{
	ZoneScopedC(RandomUniqueColor());

	ID	vs = m_resourceManager.AddShader("Default_VS", "Default_VS", ShaderType::VERTEX_SHADER),
		ps = m_resourceManager.AddShader("PBR_PS", "PBR_PS", ShaderType::PIXEL_SHADER),
		mesh = m_resourceManager.AddMesh("res/ObjectFiles/", "teapot.obj", "Teapot"),
		mesh2 = m_resourceManager.AddMesh("res/ObjectFiles/", "Cube.obj", "Cube"),
		sponza = m_resourceManager.AddMesh("res/ObjectFiles/", "sponza_1K_noDecals.glb", "Sponza"),
		tex = m_resourceManager.AddTexture("res/Textures/Magenta.png", "Grid"),
		tex2 = m_resourceManager.AddTexture("res/Textures/Spaceships/Imperial_Orange.png", "ImperialOrange"),
		samp = m_resourceManager.AddSampler("Sampler", D3D11_TEXTURE_ADDRESS_WRAP);

	csGaussianV = m_resourceManager.AddShader("post_effects/Gaussian_CS", "GaussianV_CS", ShaderType::COMPUTE_SHADER);
	csGaussianH = m_resourceManager.AddShader("post_effects/Gaussian_CS", "GaussianH_CS", ShaderType::COMPUTE_SHADER);
	csGaussianV_S = m_resourceManager.AddShader("post_effects/Gaussian_CS_S", "GaussianV_CS_S", ShaderType::COMPUTE_SHADER);
	csGaussianH_S = m_resourceManager.AddShader("post_effects/Gaussian_CS_S", "GaussianH_CS_S", ShaderType::COMPUTE_SHADER);

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
		defaultPipeline = CreatePipeline();

		defaultPipeline->SetShader(vs, ShaderType::VERTEX_SHADER);
		std::pair<const char*, DXGI_FORMAT> layout[] = {
			std::make_pair("POSITION", DXGI_FORMAT_R32G32B32_FLOAT),
			std::make_pair("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT),
			std::make_pair("UV", DXGI_FORMAT_R32G32_FLOAT),
			std::make_pair("TANGENT", DXGI_FORMAT_R32G32B32_FLOAT),
			std::make_pair("BITANGENT", DXGI_FORMAT_R32G32B32_FLOAT)
		};
		defaultPipeline->AddInputLayout(layout, 5, m_resourceManager);

		defaultPipeline->SetShader(ps, ShaderType::PIXEL_SHADER);
		DX::XMFLOAT4X4 w, vp;
		DX::XMStoreFloat4x4(&w, DX::XMMatrixIdentity());
		DX::XMStoreFloat4x4(&vp, DX::XMMatrixIdentity());
		DX::XMFLOAT4X4 matrices[2] = { w, vp };
		defaultPipeline->AddConstantBuffer(0, matrices, 2 * sizeof(DX::XMFLOAT4X4), vs, m_resourceManager);

		defaultPipeline->SetShader(csGaussianV, ShaderType::COMPUTE_SHADER);
		gaussianSettings.direction = 0;
		defaultPipeline->AddConstantBuffer(0, (void*)&gaussianSettings, sizeof(GaussianSettings), csGaussianV, m_resourceManager);

		defaultPipeline->SetShader(csGaussianH, ShaderType::COMPUTE_SHADER);
		gaussianSettings.direction = 1;
		defaultPipeline->AddConstantBuffer(0, (void*)&gaussianSettings, sizeof(GaussianSettings), csGaussianH, m_resourceManager);

		defaultPipeline->SetRasterizer(new RasterizerD3D11());

		defaultPipeline->AddEntity(entSponza);

		defaultPipeline->SetGlobalLighAmbiance(0.01f);
		defaultPipeline->AddLight(light);

		defaultPipeline->Disable();

		m_renderer.AddPipeline(defaultPipeline);
	}



	// Specialized pipeline
	{
		specializedPipeline = CreatePipeline();

		specializedPipeline->SetShader(vs, ShaderType::VERTEX_SHADER);
		std::pair<const char*, DXGI_FORMAT> layout[] = {
			std::make_pair("POSITION", DXGI_FORMAT_R32G32B32_FLOAT),
			std::make_pair("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT),
			std::make_pair("UV", DXGI_FORMAT_R32G32_FLOAT),
			std::make_pair("TANGENT", DXGI_FORMAT_R32G32B32_FLOAT),
			std::make_pair("BITANGENT", DXGI_FORMAT_R32G32B32_FLOAT)
		};
		specializedPipeline->AddInputLayout(layout, 5, m_resourceManager);

		specializedPipeline->SetShader(ps, ShaderType::PIXEL_SHADER);
		DX::XMFLOAT4X4 w, vp;
		DX::XMStoreFloat4x4(&w, DX::XMMatrixIdentity());
		DX::XMStoreFloat4x4(&vp, DX::XMMatrixIdentity());
		DX::XMFLOAT4X4 matrices[2] = { w, vp };
		specializedPipeline->AddConstantBuffer(0, matrices, 2 * sizeof(DX::XMFLOAT4X4), vs, m_resourceManager);

		specializedPipeline->SetShader(csGaussianV_S, ShaderType::COMPUTE_SHADER);
		gaussianSettings.direction = 0;
		specializedPipeline->AddConstantBuffer(0, (void*)&gaussianSettings, sizeof(GaussianSettings), csGaussianV_S, m_resourceManager);

		specializedPipeline->SetShader(csGaussianH_S, ShaderType::COMPUTE_SHADER);
		gaussianSettings.direction = 1;
		specializedPipeline->AddConstantBuffer(0, (void*)&gaussianSettings, sizeof(GaussianSettings), csGaussianH_S, m_resourceManager);

		specializedPipeline->SetRasterizer(new RasterizerD3D11());

		specializedPipeline->AddEntity(entSponza);

		specializedPipeline->SetGlobalLighAmbiance(0.01f);
		specializedPipeline->AddLight(light);

		specializedPipeline->Disable();
		m_renderer.AddPipeline(specializedPipeline);
	}
}

void SceneGaussian::Unload()
{
}

void SceneGaussian::Update()
{
	ZoneScopedC(RandomUniqueColor());

	static unsigned int frame = 0;
	static int stage = 0;

	switch (stage)
	{
	case 0: // Warmup
		if (frame == 100)
		{
			stage = 1;
			frame = 0;
			return;
		}
		break;
	case 1: // Default
		if (Benchmark(frame, false))
		{
			stage = 2;
			frame = 0;
		}
		break;
	case 2: // Warmup 2
		if (frame == 100)
		{
			stage = 3;
			frame = 0;
			return;
		}
		break;
	case 3: // Specialized
		if (Benchmark(frame, true))
		{
			stage = 4;
			frame = 0;
		}
		break;
	default:
		break;
	}

	frame++;
}

void SceneGaussian::InternalRenderUI()
{
	ZoneScopedC(RandomUniqueColor());

	ImGui::Begin("Settings");

	int oldValue = gaussianSettings.radius;
	if (ImGui::InputInt("Radius", &gaussianSettings.radius, 1, 1))
	{
		int diff = oldValue - gaussianSettings.radius;
		gaussianSettings.radius = std::clamp(oldValue - (diff*2), 3, 9);

		if (defaultPipeline->IsActive())
		{
			gaussianSettings.direction = 0;
			defaultPipeline->UpdateConstantBuffer(0, (void*)&gaussianSettings, csGaussianV, m_resourceManager);
			gaussianSettings.direction = 1;
			defaultPipeline->UpdateConstantBuffer(0, (void*)&gaussianSettings, csGaussianH, m_resourceManager);
		}
		else
		{
			ShaderD3D11* shader = m_resourceManager.GetShaderMutable(csGaussianV_S);
			shader->SetMacroValue("BLUR_RADIUS", std::to_string(gaussianSettings.radius).c_str());
			shader->Recompile();

			shader = m_resourceManager.GetShaderMutable(csGaussianH_S);
			shader->SetMacroValue("BLUR_RADIUS", std::to_string(gaussianSettings.radius).c_str());
			shader->Recompile();
		}
	}

	ImGui::End();
}

bool SceneGaussian::Benchmark(unsigned int frame, bool specialized)
{
	if (!specialized)
	{
		if (frame == 0)
		{
			defaultPipeline->Enable();
			gaussianSettings.radius = 3;

			gaussianSettings.direction = 0;
			defaultPipeline->UpdateConstantBuffer(0, (void*)&gaussianSettings, csGaussianV, m_resourceManager);
			gaussianSettings.direction = 1;
			defaultPipeline->UpdateConstantBuffer(0, (void*)&gaussianSettings, csGaussianH, m_resourceManager);
		}


		if (frame == 500)
		{
			gaussianSettings.radius = 5;

			gaussianSettings.direction = 0;
			defaultPipeline->UpdateConstantBuffer(0, (void*)&gaussianSettings, csGaussianV, m_resourceManager);
			gaussianSettings.direction = 1;
			defaultPipeline->UpdateConstantBuffer(0, (void*)&gaussianSettings, csGaussianH, m_resourceManager);
		}


		if (frame == 1000)
		{
			gaussianSettings.radius = 7;

			gaussianSettings.direction = 0;
			defaultPipeline->UpdateConstantBuffer(0, (void*)&gaussianSettings, csGaussianV, m_resourceManager);
			gaussianSettings.direction = 1;
			defaultPipeline->UpdateConstantBuffer(0, (void*)&gaussianSettings, csGaussianH, m_resourceManager);
		}

		if (frame == 1500)
		{
			gaussianSettings.radius = 9;

			gaussianSettings.direction = 0;
			defaultPipeline->UpdateConstantBuffer(0, (void*)&gaussianSettings, csGaussianV, m_resourceManager);
			gaussianSettings.direction = 1;
			defaultPipeline->UpdateConstantBuffer(0, (void*)&gaussianSettings, csGaussianH, m_resourceManager);
		}

		if (frame == 2000)
		{
			defaultPipeline->Disable();
			return true;
		}
	}

	if (specialized)
	{
		if (frame == 0)
		{
			specializedPipeline->Enable();

			ShaderD3D11* shader = m_resourceManager.GetShaderMutable(csGaussianV_S);
			shader->SetMacroValue("BLUR_RADIUS", "3");
			shader->Recompile();

			shader = m_resourceManager.GetShaderMutable(csGaussianH_S);
			shader->SetMacroValue("BLUR_RADIUS", "3");
			shader->Recompile();
		}


		if (frame == 500)
		{
			ShaderD3D11* shader = m_resourceManager.GetShaderMutable(csGaussianV_S);
			shader->SetMacroValue("BLUR_RADIUS", "5");
			shader->Recompile();

			shader = m_resourceManager.GetShaderMutable(csGaussianH_S);
			shader->SetMacroValue("BLUR_RADIUS", "5");
			shader->Recompile();
		}


		if (frame == 1000)
		{
			ShaderD3D11* shader = m_resourceManager.GetShaderMutable(csGaussianV_S);
			shader->SetMacroValue("BLUR_RADIUS", "7");
			shader->Recompile();

			shader = m_resourceManager.GetShaderMutable(csGaussianH_S);
			shader->SetMacroValue("BLUR_RADIUS", "7");
			shader->Recompile();
		}

		if (frame == 1500)
		{
			ShaderD3D11* shader = m_resourceManager.GetShaderMutable(csGaussianV_S);
			shader->SetMacroValue("BLUR_RADIUS", "9");
			shader->Recompile();

			shader = m_resourceManager.GetShaderMutable(csGaussianH_S);
			shader->SetMacroValue("BLUR_RADIUS", "9");
			shader->Recompile();
		}

		if (frame == 2000)
		{
			specializedPipeline->Disable();
			return true;
		}
	}

	return false;
}

