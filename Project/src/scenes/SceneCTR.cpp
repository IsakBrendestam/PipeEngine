#include "stdafx.h"

#include "SceneCTR.h"


struct SettingsCTR
{
	float scanlineSettings[3];
	float distortion;
	UINT  windowSize[2];
	float vignette;
	float shiftRGB;
	double time;
} settingsCTR;


SceneCTR::~SceneCTR()
{
	ZoneScopedC(RandomUniqueColor());
}

void SceneCTR::Load()
{
	ZoneScopedC(RandomUniqueColor());

	settingsCTR.scanlineSettings[0] = 0.3f;
	settingsCTR.scanlineSettings[1] = 400.0f;
	settingsCTR.scanlineSettings[2] = 1.8f;
	settingsCTR.windowSize[0] = g_engineSettings.renderResolution[0];
	settingsCTR.windowSize[1] = g_engineSettings.renderResolution[1];
	settingsCTR.distortion = 0.03f;
	settingsCTR.vignette = 0.06f;
	settingsCTR.shiftRGB = 0.003f;
	settingsCTR.time = 0.0f;

	ID vs = m_resourceManager.AddShader("Color_VS", "Color_VS", ShaderType::VERTEX_SHADER),
		ps = m_resourceManager.AddShader("Color_PS", "Color_PS", ShaderType::PIXEL_SHADER),
		mesh = m_resourceManager.AddMesh("res/ObjectFiles/", "teapot.obj", "Teapot"),
		mesh2 = m_resourceManager.AddMesh("res/ObjectFiles/", "Cube.obj", "Cube"),
		tex = m_resourceManager.AddTexture("res/Textures/Magenta.png", "Grid"),
		tex2 = m_resourceManager.AddTexture("res/Textures/Spaceships/Imperial_Orange.png", "ImperialOrange"),
		samp = m_resourceManager.AddSampler("Sampler", D3D11_TEXTURE_ADDRESS_WRAP);

	m_cs = m_resourceManager.AddShader("PostEffectCTR_CS", "PostEffects_CS", ShaderType::COMPUTE_SHADER);
	
	{
		Pipeline* pipeline = CreatePipeline();

		pipeline->SetShader(vs, ShaderType::VERTEX_SHADER);
		pipeline->SetShader(ps, ShaderType::PIXEL_SHADER);

		pipeline->SetRasterizer(new RasterizerD3D11());

		DX::XMFLOAT4X4 w, vp;
		DX::XMStoreFloat4x4(&w, DX::XMMatrixIdentity());
		DX::XMStoreFloat4x4(&vp, DX::XMMatrixIdentity());
		DX::XMFLOAT4X4 matrices[2] = { w, vp };
		pipeline->AddConstantBuffer(0, matrices, 2*sizeof(DX::XMFLOAT4X4), vs, m_resourceManager);

		std::pair<const char*, DXGI_FORMAT> layout[] = {
			std::make_pair("POSITION", DXGI_FORMAT_R32G32B32_FLOAT),
			std::make_pair("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT)
		};
		pipeline->AddInputLayout(layout, 2, m_resourceManager);

		{
			Entity* ent = CreateEntity();
			ent->SetMesh(mesh);
			ent->SetSampler(samp);
			ent->SetTransform(Transform({ 0, 4, -4 }, { 0, 0, 0 }, { 0.1f, 0.1f, 0.1f }));

			pipeline->AddEntity(ent);
		}

		{
			Entity* ent = CreateEntity();
			ent->SetMesh(mesh2);
			ent->SetSampler(samp);
			ent->SetTransform(Transform({ 5, 4, 10 }, { 0, 0, 0 }, { 1, 1, 1 }));

			pipeline->AddEntity(ent);
		}

		//pipeline->Disable();

		m_renderer.AddPipeline(pipeline);
	}


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

		pipeline->SetShader(m_cs, ShaderType::COMPUTE_SHADER);
		pipeline->AddConstantBuffer(0, &settingsCTR, 2 * sizeof(SettingsCTR), m_cs, m_resourceManager);

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

		m_renderer.AddPipeline(pipeline);
	}
}

void SceneCTR::Unload()
{
}

void SceneCTR::Update()
{
	ZoneScopedC(RandomUniqueColor());

	settingsCTR.time += g_time.DeltaTime<double>() / 1000;
	GetPipeline(0)->UpdateConstantBuffer(0, &settingsCTR, m_cs, m_resourceManager);
}

void SceneCTR::InternalRenderUI()
{
	ZoneScopedC(RandomUniqueColor());
}
