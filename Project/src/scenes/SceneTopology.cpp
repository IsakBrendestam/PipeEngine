#include "stdafx.h"
#include "SceneTopology.h"

SceneTopology::~SceneTopology()
{
	ZoneScopedC(RandomUniqueColor());
}

void SceneTopology::Load()
{
	ZoneScopedC(RandomUniqueColor());

	ID  vs = m_resourceManager.AddShader("Color_VS", "Color_VS", ShaderType::VERTEX_SHADER),
		ps = m_resourceManager.AddShader("Color_PS", "Color_PS", ShaderType::PIXEL_SHADER),

		mesh = m_resourceManager.AddMesh("res/ObjectFiles/", "lucy.ply", "Lucy"),

		tex = m_resourceManager.AddTexture("res/Textures/LightBlue.png", "LightBlue", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB),
		tex2 = m_resourceManager.AddTexture("res/Textures/Gray.png", "Gray", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB),
		tex3 = m_resourceManager.AddTexture("res/Textures/DarkGray.png", "DarkGray", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB),

		samp = m_resourceManager.AddSampler("Sampler", D3D11_TEXTURE_ADDRESS_WRAP);
	
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

		// Wall
		{
			Entity* ent = CreateEntity();
			ent->SetMesh(mesh);
			ent->SetSampler(samp);
			ent->SetTransform(Transform({ 5, 4, 4 }, { M_PI/2.0f, 0, 0 }, { 0.02f, 0.02f, 0.02f }));
			ent->SetMaterial({
				.albedoMap = tex3
			});

			pipeline->AddEntity(ent);
		}

		// Room
		ID tempMesh = m_resourceManager.AddMesh("res/ObjectFiles/", "InvertedCube.obj", "InvertedCube");
		{
			Entity* ent = CreateEntity();
			ent->SetMesh(tempMesh);
			ent->SetSampler(samp);
			ent->SetTransform(Transform({ 0, 10, 0 }, { 0, 0, 0 }, { 30, 20, 30 }));
			ent->SetMaterial({
				.albedoMap = tex2
			});

			pipeline->AddEntity(ent);
		}

		m_renderer.AddPipeline(pipeline);
	}
}

void SceneTopology::Unload()
{
}

void SceneTopology::Update()
{
	ZoneScopedC(RandomUniqueColor());
}

void SceneTopology::InternalRenderUI()
{
	ZoneScopedC(RandomUniqueColor());

	ImGui::Begin("Settings");

	ImGui::End();
}
