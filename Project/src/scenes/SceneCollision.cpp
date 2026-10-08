#include "stdafx.h"

#include "SceneCollision.h"

using namespace Collisions;

SceneCollision::~SceneCollision()
{
	ZoneScopedC(RandomUniqueColor());
}

void SceneCollision::Load()
{
	ZoneScopedC(RandomUniqueColor());

	ID vs = m_resourceManager.AddShader("Color_VS", "Color_VS", ShaderType::VERTEX_SHADER),
		ps = m_resourceManager.AddShader("Color_PS", "Color_PS", ShaderType::PIXEL_SHADER),
		cs = m_resourceManager.AddShader("Default_CS", "Default_CS", ShaderType::COMPUTE_SHADER),
		mesh = m_resourceManager.AddMesh("res/ObjectFiles/", "teapot.obj", "Teapot");

	   m_meshCube = m_resourceManager.AddMesh("res/ObjectFiles/", "CubeChecker.obj", "CubeChecker");
	   m_texBlue = m_resourceManager.AddTexture("res/Textures/LightBlue.png", "LightBlue", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB),
	   m_texGray = m_resourceManager.AddTexture("res/Textures/Gray.png", "Gray", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB),
	   m_texGreen = m_resourceManager.AddTexture("res/Textures/Green.png", "Green", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB),
	   m_texChecker = m_resourceManager.AddTexture("res/Textures/Checker.png", "Checker", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB),
	   m_sampler = m_resourceManager.AddSampler("Sampler", D3D11_TEXTURE_ADDRESS_WRAP);
	
	m_resourceManager.AddShader("PostEffectCTR_CS", "PostEffectCTR_CS", ShaderType::COMPUTE_SHADER);

	{
		vs = m_resourceManager.AddShader("Texture_VS", "Texture_VS", ShaderType::VERTEX_SHADER);
		ps = m_resourceManager.AddShader("Texture_PS", "Texture_PS", ShaderType::PIXEL_SHADER);

		m_pipeline = CreatePipeline();

		m_pipeline->SetShader(vs, ShaderType::VERTEX_SHADER);
		std::pair<const char*, DXGI_FORMAT> layout[] = {
			std::make_pair("POSITION", DXGI_FORMAT_R32G32B32_FLOAT),
			std::make_pair("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT),
			std::make_pair("UV", DXGI_FORMAT_R32G32_FLOAT)
		};
		m_pipeline->AddInputLayout(layout, 3, m_resourceManager);

		m_pipeline->SetShader(ps, ShaderType::PIXEL_SHADER);
		DX::XMFLOAT4X4 w, vp;
		DX::XMStoreFloat4x4(&w, DX::XMMatrixIdentity());
		DX::XMStoreFloat4x4(&vp, DX::XMMatrixIdentity());
		DX::XMFLOAT4X4 matrices[2] = { w, vp };
		m_pipeline->AddConstantBuffer(0, matrices, 2 * sizeof(DX::XMFLOAT4X4), vs, m_resourceManager);

		m_pipeline->AddLight(PointLight({ 0, 10, 0 }, { 1, 1, 1 }, 150, 1, 8));

		m_pipeline->SetShader(cs, ShaderType::COMPUTE_SHADER);


		m_pipeline->SetRasterizer(new RasterizerD3D11());

		{
			Entity* ent = CreateEntity();
			ent->SetMesh(mesh);
			ent->SetSampler(m_sampler);
			ent->SetTransform(Transform({ -3, 0, -4 }, { 0, 0, 0 }, { 0.1f, 0.1f, 0.1f }));
			ent->SetMaterial(MaterialMaps(m_texBlue));
			
			m_pipeline->AddEntity(ent);
		}

		for (float y = 3; y < 50; y *= 2.5f)
		{
			float offset = fmodf(y, 2) * 1.5f;
			for (float x = -3 + offset; x < 10; x += 2.25f)
				SpawnColliderEntity({ x, y, 0 });
		}

		// Room
		ID tempMesh = m_resourceManager.AddMesh("res/ObjectFiles/", "InvertedCube.obj", "InvertedCube");
		{
			Entity* ent = CreateEntity();
			ent->SetMesh(tempMesh);
			ent->SetSampler(m_sampler);
			ent->SetTransform(Transform({ 0, 10, 0 }, { 0, 0, 0 }, { 30, 20, 30 }));
			ent->SetMaterial(MaterialMaps(m_texGray));

			Collider* c = new AABB({ 0, -20, 0 }, { 1, 0.001, 1 }, ColliderTags::STATIC_TAG);
			ent->SetCollider(c);

			m_pipeline->AddEntity(ent);
		}

		m_renderer.AddPipeline(m_pipeline);
	}
}

void SceneCollision::Unload()
{
}

void SceneCollision::Update()
{
	ZoneScopedC(RandomUniqueColor());

	spawnTime += g_time.DeltaTime();
	if (InputManager::GetKeyState(SDLK_f) && spawnTime > spawnRate)
	{
		spawnTime = 0;
		SpawnColliderEntity(g_cameraManager.GetCamera()->GetPosition(), true);
	}

	std::vector<Entity*> entities;
	GetEntities(entities);

	for (auto& entity : entities)
	{
		entity->Update();
	}
}

void SceneCollision::InternalRenderUI()
{
	ZoneScopedC(RandomUniqueColor());

	ImGui::Begin("Settings");


	ImGui::End();
}

void SceneCollision::SpawnColliderEntity(const DX::XMFLOAT3& position, bool addImpulse)
{
	static int count = 0;

	DebugLog(std::format("Rigidbodies {}", ++count));

	Entity* ent = CreateEntity();
	ent->SetMesh(m_meshCube);
	ent->SetSampler(m_sampler);
	ent->SetTransform(Transform(position, { 0, 0, 0 }, { 1, 1, 1 }));
	ent->SetMaterial(MaterialMaps(m_texChecker));

	AABB aabb({ 0,0,0 }, { 1,1,1 });
	Collider* c = new OBB(aabb);
	ent->SetCollider(c);
	c = ent->GetCollider();
	c->AddOnCollisionEnterCallback([ent, this](const CollisionData& data) {
		ent->SetMaterial(MaterialMaps(m_texGreen));
	});
	c->AddOnCollisionExitCallback([ent, this]() {
		ent->SetMaterial(MaterialMaps(m_texBlue));
	});


	DX::XMVECTOR v = DX::XMLoadFloat3(&g_cameraManager.GetControllCamera()->GetForward());
	ent->SetRigidbody(new Rigidbody(ent, 1.0f));
	if (addImpulse)
		ent->GetRigidbody()->AddImpulse(DX::XMVectorScale(v, 22));

	m_pipeline->AddEntity(ent);
}
