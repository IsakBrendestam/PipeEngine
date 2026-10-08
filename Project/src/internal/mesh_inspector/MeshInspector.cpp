#include "stdafx.h"
#include "MeshInspector.h"

struct MeshDataBuffer
{
    DX::XMFLOAT3 aabbCenter;
    float aabbMaxExtent;
};

MeshInspector::~MeshInspector()
{
	delete m_pipeline;
	delete m_entity;
}

void MeshInspector::Initialize()
{
	// Setup scene
	m_vs = m_resourceManager.AddShader("Debug_VS", "Debug_VS", ShaderType::VERTEX_SHADER);
	m_ps = m_resourceManager.AddShader("Debug_PS", "Debug_PS", ShaderType::PIXEL_SHADER);

	// RTV
	UINT width = 1920, height = 1080;
	m_renderTarget.Initialize(width, height);

	m_entity = new Entity();
	m_entity->SetTransform(Transform({ 0, 0, 0 }, { 0, 0, 0 }, { 1, 1, 1 }));

	// Pipeline
	{
		m_pipeline = new Pipeline();
		m_pipeline->SetRenderTarget(&m_renderTarget);
		m_pipeline->SetViewport({
			.TopLeftX = 0,
			.TopLeftY = 0,
			.Width = static_cast<float>(width),
			.Height = static_cast<float>(height),
			.MinDepth = 0,
			.MaxDepth = 1,
		});

		m_pipeline->SetShader(m_vs, ShaderType::VERTEX_SHADER);
		m_pipeline->SetShader(m_ps, ShaderType::PIXEL_SHADER);

		std::pair<const char*, DXGI_FORMAT> layout[] = {
			std::make_pair("POSITION", DXGI_FORMAT_R32G32B32_FLOAT),
			std::make_pair("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT),
			std::make_pair("UV", DXGI_FORMAT_R32G32_FLOAT),
			std::make_pair("TANGENT", DXGI_FORMAT_R32G32B32_FLOAT),
			std::make_pair("BITANGENT", DXGI_FORMAT_R32G32B32_FLOAT)
		};
		m_pipeline->AddInputLayout(layout, 5, m_resourceManager);

		DX::XMFLOAT4X4 w, vp;
		DX::XMStoreFloat4x4(&w, DX::XMMatrixIdentity());
		DX::XMStoreFloat4x4(&vp, DX::XMMatrixIdentity());
		DX::XMFLOAT4X4 matrices[2] = { w, vp };
		m_pipeline->AddConstantBuffer(0, matrices, 2 * sizeof(DX::XMFLOAT4X4), m_vs, m_resourceManager);

		MeshDataBuffer meshData = {};
		m_pipeline->AddConstantBuffer(1, &meshData, sizeof(MeshDataBuffer), m_vs, m_resourceManager);

		m_pipeline->AddLight(PointLight({ 4, 9, -14 }, { 1, 1, 1 }, 150, 1, 8));

		m_pipeline->SetRasterizer(new RasterizerD3D11());

		m_pipeline->SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });

		m_pipeline->AddEntity(m_entity);
	}

	// Renderer
	m_renderer.Initialize();
	m_renderer.AddPipeline(m_pipeline);

	// Camera
	m_camera.Initialize({ 
		.fovAngleY = 45.0f, 
		.aspectRatio = (float)width / (float)height,
		.nearZ = 0.1f,
		.farZ = 100.0f 
	},
	{0, 0, -4.0f});
}

void MeshInspector::Update()
{
	if (!m_active) return;

	if (m_windowFocused)
		ChangeView();
}

void MeshInspector::Render(const ResourceManager& rm, Entity* entity)
{
	if (!m_active) return;
	if (!entity) return;
	
	m_resourceManager.Append(rm);

	if (CheckEntityChanged(entity))
		SetEntity(entity);

	m_renderer.Render(&m_camera, &m_resourceManager);
}

void MeshInspector::RenderUI()
{
	ZoneScopedC(RandomUniqueColor());

	if (!m_active)
		return;

	ImGui::Begin("Mesh Inspector", &m_active);

	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows))
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			ImGui::SetWindowFocus();

	m_windowFocused = ImGui::IsWindowFocused();

	ImGui::Image(
		(ImTextureID)m_renderTarget.GetSRV(),
		ImVec2(m_renderTarget.GetWidth()/4.0f, m_renderTarget.GetHeight()/4.0f)
	);

	DX::XMFLOAT3 cameraPosition = m_camera.GetPosition();
	if (ImGui::DragFloat3("Camera Position", &cameraPosition.x, 0.1f))
		m_camera.SetPosition(cameraPosition);


	ImGui::End();
}

void MeshInspector::SetActive()
{
	m_active = true;
}

void MeshInspector::SetEntity(const Entity* entity)
{
	ID meshID = entity->GetMesh();
	m_entity->SetMesh(meshID);
	m_entity->SetMaterial(entity->GetMaterial());

	DX::XMFLOAT3 tempMin = m_resourceManager.GetMesh(meshID)->GetBoundingBoxMin();
	DX::XMFLOAT3 tempMax = m_resourceManager.GetMesh(meshID)->GetBoundingBoxMax();
	DX::XMVECTOR aabbMin = DX::XMLoadFloat3(&tempMin);
	DX::XMVECTOR aabbMax = DX::XMLoadFloat3(&tempMax);

	DX::XMVECTOR aabbCenterVec = DX::XMVectorScale(DX::XMVectorAdd(aabbMin, aabbMax), 0.5f);
    DX::XMFLOAT3 aabbCenter;
	DX::XMStoreFloat3(&aabbCenter, aabbCenterVec);

	DX::XMVECTOR aabbExtents = DX::XMVectorScale(DX::XMVectorSubtract(aabbMax, aabbMin), 0.5f);
    float aabbMaxExtent = std::max<float>(DX::XMVectorGetX(aabbExtents), std::max<float>(DX::XMVectorGetY(aabbExtents), DX::XMVectorGetZ(aabbExtents)));

	MeshDataBuffer meshData = {
		aabbCenter,
		aabbMaxExtent
	};

	m_pipeline->UpdateConstantBuffer(1, &meshData, m_vs, m_resourceManager);
}

bool MeshInspector::CheckEntityChanged(const Entity* entity) const
{
	return m_entity->GetMesh() != entity->GetMesh() ||
		   m_entity->GetMaterial() != entity->GetMaterial();
}

void MeshInspector::ChangeView()
{
	MouseState ms = InputManager::GetMouseState();
	float deltaTime = g_time.DeltaTime();

	// Rotation
	if (g_engineSettings.cameraMouseMovement || ms.rightButtonDown)
	{
		InputManager::LockMouse();
		m_entity->GetTransform().Rotate({-2 * ms.deltaY * deltaTime, -2 * ms.deltaX * deltaTime, 0});
		m_wasLocked = true;
	}
	else
	{
		if (m_wasLocked)
		{
			InputManager::UnlockMouse();
			m_wasLocked = false;
		}
	}

	if (ms.verticalScroll != 0)
		m_camera.MoveForward(100.0f * ms.verticalScroll * deltaTime);
}
