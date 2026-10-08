#include "stdafx.h"

#include "SceneSobel.h"

SceneSobel::~SceneSobel()
{
	ZoneScopedC(RandomUniqueColor());
}

void SceneSobel::Load()
{
	ZoneScopedC(RandomUniqueColor());

	ID vs = m_resourceManager.AddShader("Color_VS", "Color_VS", ShaderType::VERTEX_SHADER),
		ps = m_resourceManager.AddShader("Color_PS", "Color_PS", ShaderType::PIXEL_SHADER),
		cs = m_resourceManager.AddShader("SobelFilter_CS", "SobelFilter_CS", ShaderType::COMPUTE_SHADER),
		mesh = m_resourceManager.AddMesh("res/ObjectFiles/", "teapot.obj", "Teapot"),
		mesh2 = m_resourceManager.AddMesh("res/ObjectFiles/", "Cube.obj", "Cube"),
		tex = m_resourceManager.AddTexture("res/Textures/LightBlue.png", "LightBlue"),
		tex2 = m_resourceManager.AddTexture("res/Textures/Spaceships/Imperial_Orange.png", "ImperialOrange"),
		samp = m_resourceManager.AddSampler("Sampler", D3D11_TEXTURE_ADDRESS_WRAP);
	
	m_resourceManager.AddShader("PostEffectCTR_CS", "PostEffectCTR_CS", ShaderType::COMPUTE_SHADER);
	m_resourceManager.AddShader("Default_CS", "Default_CS", ShaderType::COMPUTE_SHADER);

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

		pipeline->ClearRenderTarget(false);

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

		pipeline->AddLight(PointLight({ 0, 0, 0 }, { 1, 1, 1 }, 10, 1, 8));

		pipeline->SetShader(cs, ShaderType::COMPUTE_SHADER);


		pipeline->SetRasterizer(new RasterizerD3D11());

		{
			Entity* ent = CreateEntity();
			ent->SetMesh(mesh);
			ent->SetSampler(samp);
			ent->SetTransform(Transform({ -3, 4, -4 }, { 0, 0, 0 }, { 0.1f, 0.1f, 0.1f }));
			ent->SetMaterial({ .albedoMap = tex });

			pipeline->AddEntity(ent);
		}

		{
			Entity* ent = CreateEntity();
			ent->SetMesh(mesh2); 
			ent->SetSampler(samp);
			ent->SetTransform(Transform({ 5, 4, 4 }, { 0, 0, 0 }, { 1, 1, 1 }));
			ent->SetMaterial({ .albedoMap = tex });

			pipeline->AddEntity(ent);
		}

		/*ID tempMesh = m_resourceManager.AddMesh("res/ObjectFiles/", "Antenna.obj", "Antenna");
		{
			Entity* ent = CreateEntity();
			ent->SetMesh(tempMesh);
			ent->SetSampler(samp);
			ent->SetTransform(Transform({ 5, 4, 4 }, { 0, 0, 0 }, { 1, 1, 1 }));
			ent->SetMaterial(MaterialMaps(tex));

			pipeline->AddEntity(ent);
		}
		*/

		m_renderer.AddPipeline(pipeline);
	}
}

void SceneSobel::Unload()
{
}

void SceneSobel::Update()
{
	ZoneScopedC(RandomUniqueColor());
}

void SceneSobel::InternalRenderUI()
{
	ZoneScopedC(RandomUniqueColor());
}
