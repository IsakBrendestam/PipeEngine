#include "stdafx.h"

#include "SceneField.h"

SceneField::~SceneField()
{
	ZoneScopedC(RandomUniqueColor());
}

void SceneField::Load()
{
	ZoneScopedC(RandomUniqueColor());

	ID vs = m_resourceManager.AddShader("Color_VS", "Color_VS", ShaderType::VERTEX_SHADER),
		ps = m_resourceManager.AddShader("Color_PS", "Color_PS", ShaderType::PIXEL_SHADER),
		cs = m_resourceManager.AddShader("Default_CS", "Default_CS", ShaderType::COMPUTE_SHADER),
		mesh = m_resourceManager.AddMesh("res/ObjectFiles/", "teapot.obj", "Teapot"),
		mesh2 = m_resourceManager.AddMesh("res/ObjectFiles/", "Cube.obj", "Cube"),
		tex = m_resourceManager.AddTexture("res/Textures/LightBlue.png", "LightBlue", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB),
		tex2 = m_resourceManager.AddTexture("res/Textures/Gray.png", "Gray", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB),
		samp = m_resourceManager.AddSampler("Sampler", D3D11_TEXTURE_ADDRESS_WRAP);
	
	m_resourceManager.AddShader("PostEffectCTR_CS", "PostEffectCTR_CS", ShaderType::COMPUTE_SHADER);

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

		pipeline->SetShader(cs, ShaderType::COMPUTE_SHADER);


		pipeline->SetRasterizer(new RasterizerD3D11());

		{
			Entity* ent = CreateEntity();
			ent->SetMesh(mesh);
			ent->SetSampler(samp);
			ent->SetTransform(Transform({ -3, 0, -4 }, { 0, 0, 0 }, { 0.1f, 0.1f, 0.1f }));
			ent->SetMaterial(MaterialMaps(tex));
			
			pipeline->AddEntity(ent);
		}

		{
			Entity* ent = CreateEntity();
			ent->SetMesh(mesh2);
			ent->SetSampler(samp);
			ent->SetTransform(Transform({ 5, 0, 4 }, { 0, 0, 0 }, { 1, 1, 1 }));
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

		// Spehere
		
		ID sphere = m_resourceManager.AddMesh("res/ObjectFiles/", "HighResSphere.obj", "HighResSphere");
		tex = m_resourceManager.AddTexture("res/Textures/Turqosue.png", "Turqosue", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
		{
			Entity* ent = CreateEntity();
			ent->SetMesh(sphere);
			ent->SetSampler(samp);
			ent->SetTransform(Transform({ 3, -10, 7 }, { 0, 0, 0 }, { 10, 10, 10 }));
			ent->SetMaterial(MaterialMaps(tex));

			pipeline->AddEntity(ent);
		}

		m_renderer.AddPipeline(pipeline);
	}
}

void SceneField::Unload()
{
}

void SceneField::Update()
{
	ZoneScopedC(RandomUniqueColor());
}

void SceneField::InternalRenderUI()
{
	ZoneScopedC(RandomUniqueColor());

	ImGui::Begin("Settings");


	ImGui::End();
}
