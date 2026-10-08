#include "stdafx.h"

#include "Pipeline.h"
#include "core/scene/Scene.h"

#include "ui/PipelineUI.h"

constexpr ID3D11ShaderResourceView* nullSRV[1]{ nullptr };

Pipeline::Pipeline()
{
	Initialize();
}

Pipeline::~Pipeline()
{
	ZoneScopedC(RandomUniqueColor());

	delete m_depthBuffer;
	delete m_rasterizer;
	delete m_inputLayout;
	delete m_gBuffer;
}

void Pipeline::Initialize()
{
	ZoneScopedC(RandomUniqueColor());

	// TODO: Setup the following variables
	m_rtv = DeviceManager::RenderRTV();
	m_uav = nullptr;

	m_depthBuffer = new DepthBufferD3D11(g_engineSettings.renderResolution[0], g_engineSettings.renderResolution[1]);
	m_viewport = DeviceManager::Viewport();

	m_gBuffer = new GBufferD3D11(g_engineSettings.renderResolution[0], g_engineSettings.renderResolution[1]);

	m_rasterizer = nullptr;
	m_inputLayout = nullptr;

	m_lightManager.Initialize(1);

	std::ranges::copy(g_engineSettings.clearColor, m_clearColor);
}

bool Pipeline::SetShader(ID id, ShaderType type)
{
	ZoneScopedC(RandomUniqueColor());

	switch (type)
	{
	case ShaderType::VERTEX_SHADER:
		m_vShaderID = id;
		return true;
	case ShaderType::HULL_SHADER:
		return false;
	case ShaderType::DOMAIN_SHADER:
		return false;
	case ShaderType::GEOMETRY_SHADER:
		return false;
	case ShaderType::PIXEL_SHADER:
		m_pShaderID = id;
		return true;
	case ShaderType::COMPUTE_SHADER:
		m_cShaderIDs.push_back(id);

		m_cTextures.push_back({nullptr});
		m_cTextures.back()[0] = m_gBuffer->GetSRV();

		m_cOutTextures.push_back({nullptr});
		m_cOutTextures.back()[0] = m_rtv->GetUAV();
		return true;
	default:
		return false;
	}
	return true;
}

bool Pipeline::RemoveShader(ID id, ShaderType type)
{
	switch (type)
	{
	case ShaderType::VERTEX_SHADER:
	{
		if (m_vShaderID == id)
			m_vShaderID = INVALID_ID;
		return true;
	}
	case ShaderType::HULL_SHADER:
		return false;
	case ShaderType::DOMAIN_SHADER:
		return false;
	case ShaderType::GEOMETRY_SHADER:
		return false;
	case ShaderType::PIXEL_SHADER:
	{
		if (m_pShaderID == id)
			m_pShaderID = INVALID_ID;
		return true;
	}
	case ShaderType::COMPUTE_SHADER:
	{
		auto cShaderID = std::find(m_cShaderIDs.begin(), m_cShaderIDs.end(), id);
		if (cShaderID != m_cShaderIDs.end())
			*cShaderID = INVALID_ID;
		return true;
	}
	default:
		return false;
	}
	return true;
}

void Pipeline::DeleteComputeShader(ID id)
{
	m_cShaderIDs.erase(std::remove(m_cShaderIDs.begin(), m_cShaderIDs.end(), id), m_cShaderIDs.end());
}

bool Pipeline::AddConstantBuffer(int index, void* data, size_t size, ID id, ResourceManager& resourceManager)
{
	ZoneScopedC(RandomUniqueColor());

	if (index >= MAX_BUFFERS) 
		return false;

	ShaderD3D11* shader = resourceManager.GetShaderMutable(id);
	
	if (!shader)
		return false;

	shader->AddConstantBuffer(index, data, size);

	return true;
}


bool Pipeline::UpdateConstantBuffer(int index, void* data, ID id, ResourceManager& resourceManager)
{
	ZoneScopedC(RandomUniqueColor());

	if (index >= MAX_BUFFERS) return false;

	ShaderD3D11* shader = resourceManager.GetShaderMutable(id);
	
	if (!shader)
		return false;

	shader->UpdateConstantBuffer(index, data);

	return true;
}

bool Pipeline::AddTexture(ID3D11ShaderResourceView* texture, ShaderType type, unsigned int index, unsigned int shaderIndex)
{
	if (index >= MAX_TEXTURES) return false;

	switch (type)
	{
	case ShaderType::VERTEX_SHADER:
		break;
	case ShaderType::HULL_SHADER:
		break;
	case ShaderType::DOMAIN_SHADER:
		break;
	case ShaderType::GEOMETRY_SHADER:
		break;
	case ShaderType::PIXEL_SHADER:
		break;
	case ShaderType::COMPUTE_SHADER:
		if (shaderIndex < m_cTextures.size())
			m_cTextures[shaderIndex][index] = texture;
		break;
	default:
		break;
	}

	return true;
}

bool Pipeline::SetOutTexture(ID3D11UnorderedAccessView* texture, ShaderType type, unsigned int index, unsigned int shaderIndex)
{
	if (index >= MAX_TEXTURES) return false;

	switch (type)
	{
	case ShaderType::VERTEX_SHADER:
		break;
	case ShaderType::HULL_SHADER:
		break;
	case ShaderType::DOMAIN_SHADER:
		break;
	case ShaderType::GEOMETRY_SHADER:
		break;
	case ShaderType::PIXEL_SHADER:
		break;
	case ShaderType::COMPUTE_SHADER:
		if (shaderIndex < m_cTextures.size())
			m_cOutTextures[shaderIndex][index] = texture;
		break;
	default:
		break;
	}

	return true;
}

void Pipeline::SetViewport(D3D11_VIEWPORT viewport)
{
	m_viewport = viewport;
}

void Pipeline::SetRasterizer(RasterizerD3D11* rasterizer)
{
	m_rasterizer = rasterizer;
}

void Pipeline::SetRenderTarget(const RenderTargetD3D11* rtv)
{
	m_rtv = rtv;

	delete m_depthBuffer;
	m_depthBuffer = new DepthBufferD3D11(rtv->GetWidth(), rtv->GetHeight());
}

void Pipeline::AddInputLayout(const std::pair<const char*, DXGI_FORMAT> inputElements[], unsigned int count, ResourceManager& resourceManager)
{ 
	if (m_vShaderID == INVALID_ID) return;

	const ShaderD3D11* vShader = resourceManager.GetShader(m_vShaderID);

	m_inputLayout = new InputLayoutD3D11();
	for (int i = 0; i < count; i++)
		m_inputLayout->AddInputElement(inputElements[i].first, inputElements[i].second);
	m_inputLayout->FinalizeInputLayout(vShader->GetShaderByteData(), vShader->GetShaderByteSize());
}

void Pipeline::AddEntity(Entity* entity)
{
	m_entitiesSorted = false;
	m_entities.push_back(entity);
}

void Pipeline::RemoveEntity(Entity* entity)
{
	if (!entity) return;

	m_entities.erase(std::remove(m_entities.begin(), m_entities.end(), entity), m_entities.end());
}

bool Pipeline::HasEntity(const Entity* entity) const
{
	return std::find(m_entities.begin(), m_entities.end(), entity) != m_entities.end();
}

void Pipeline::AddLight(PointLight light)
{
	m_lightManager.AddLight(light);
}

void Pipeline::AddLight(SpotLight light)
{
	m_lightManager.AddLight(light);
}

void Pipeline::AddLight(DirectionalLight light)
{
	m_lightManager.AddLight(light);
}

void Pipeline::SetGlobalLighAmbiance(float ambiance)
{
	m_lightManager.SetGlobalAmbience(ambiance);
}

const LightManager& Pipeline::GetLightManager() const
{
	return m_lightManager;
}

void Pipeline::SetClearColor(const float clearColor[4])
{
	for (int i = 0; i < 4; i++)
		m_clearColor[i] = clearColor[i];
}

void Pipeline::SetClearColor(const std::array<float, 4> clearColor)
{
	for (int i = 0; i < 4; i++)
		m_clearColor[i] = clearColor[i];
}

void Pipeline::ClearRtv()
{
	DeviceManager::Context()->ClearRenderTargetView(m_rtv->GetRTV(), m_clearColor);
}

bool Pipeline::StartRender(ResourceManager* resourceManager)
{
	ZoneScopedC(RandomUniqueColor());

	if (!IsValid()) [[unlikely]]
		return false;

	// Clear
	if (m_clearRenderTarget)
		DeviceManager::Context()->ClearRenderTargetView(m_rtv->GetRTV(), m_clearColor);

	if (HasComputeShader() && m_gBuffer)
		DeviceManager::Context()->ClearRenderTargetView(m_gBuffer->GetRTV(), m_clearColor);

	DeviceManager::Context()->ClearDepthStencilView(m_depthBuffer->GetDSV(0), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);

	//if (!m_entitiesSorted)
	{
		//std::sort(m_entities.begin(), m_entities.end());
		//m_entitiesSorted = true;
	}

	m_resourceManager = resourceManager;

	return true;
}

bool Pipeline::Render(CameraD3D11* camera)
{
	ZoneScopedC(RandomUniqueColor());
	TracyD3D11ZoneC(g_tracyContext, "Pipeline::Render", RandomUniqueColor());
	InternalScope("Pipeline Render");

	Bind(camera);

	if (!IsValid()) [[unlikely]] 
		return false;

	for (auto& ent : m_entities)
		RenderEntity(camera, ent);

	ComputePass();

	return true;
}

bool Pipeline::RenderEntity(CameraD3D11* camera, Entity* entity)
{	
	ZoneScopedC(RandomUniqueColor());

	if (!IsValid()) [[unlikely]] 
		return false;

	DX::XMFLOAT4X4 w, vp;
	DX::XMStoreFloat4x4(&w, entity->GetTransform().GetWorldMatrix());
	vp = camera->GetViewProjectionMatrix();
	DX::XMFLOAT4X4 matrices[2] = { w, vp };

	// Update tramsformation buffer
	UpdateConstantBuffer(0, matrices, m_vShaderID, *m_resourceManager);

	BindEntity(entity);

	MaterialMaps tempMaterial = entity->GetMaterial();
	bool overrideMaterial = tempMaterial != g_nullMaterialMap;
	if (overrideMaterial)
		BindMaterial(tempMaterial);

	// Draw Mesh
	const MeshD3D11* mesh = m_resourceManager->GetMesh(entity->GetMesh());
	if (mesh)
	{
		auto subMeshMaterials = mesh->GetSubMeshInfo();
		if (mesh)
			for (size_t i = 0; i < mesh->GetNrOfSubMeshes(); i++)
			{
				if (!overrideMaterial)
					BindMaterial(subMeshMaterials[i].materialMaps);
				mesh->DrawSubMesh(i);
			}
	}

	return true;
}

bool Pipeline::ComputePass()
{
	ZoneScopedC(RandomUniqueColor());
	TracyD3D11ZoneC(g_tracyContext, "Pipeline::ComputePass", RandomUniqueColor());

	ID3D11RenderTargetView* nullRTVs[1] = { nullptr };
	DeviceManager::Context()->OMSetRenderTargets(1, nullRTVs, nullptr);

	ID3D11Resource* src = nullptr;
	ID3D11Resource* dst = nullptr;

	m_rtv->GetUAV()->GetResource(&src);
	m_gBuffer->GetRTV()->GetResource(&dst);

	for (size_t i = 0; i < m_cShaderIDs.size(); i++)
	{
		ID shaderID = m_cShaderIDs[i];
		if (shaderID == INVALID_ID)
			continue;

		ZoneScopedNCD(std::format("Compute Shader (ID: {})", shaderID).c_str(), RandomUniqueColor());
		InternalScope(std::format("Compute Shader (ID: {})", shaderID));

		BindCompute(shaderID);

		DeviceManager::Context()->CSSetShaderResources(0, MAX_TEXTURES, m_cTextures[i].data());

		DeviceManager::Context()->CSSetUnorderedAccessViews(0, MAX_TEXTURES, m_cOutTextures[i].data(), nullptr);

		{
			TracyD3D11ZoneC(g_tracyContext, "Dispatch", RandomUniqueColor());
			InternalScopeGPU(std::format("Compute Shader Dispatch (ID: {})", shaderID));
			DeviceManager::Context()->Dispatch((m_rtv->GetWidth() + 7) / 8, (m_rtv->GetHeight() + 7) / 8, 1);
		}

		ID3D11UnorderedAccessView* nullUAV[1] = { nullptr };
		DeviceManager::Context()->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);

		ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
		DeviceManager::Context()->CSSetShaderResources(0, 1, nullSRV);

		if (m_cOutTextures[i][0] == m_rtv->GetUAV())
			DeviceManager::Context()->CopyResource(dst, src);
	}

	src->Release();
	dst->Release();

	return true;
}

bool Pipeline::EndRender()
{
	ZoneScopedC(RandomUniqueColor());

	for (char i = 0; i < m_textureCount; i++)
		m_boundTextures[i] = INVALID_ID;
	m_loadedMesh = INVALID_ID;
	m_loadedSampler = INVALID_ID;

	Unbind();

	m_resourceManager = nullptr;

	return true;
}

void Pipeline::Unbind()
{
	const ShaderD3D11* vShader = m_resourceManager->GetShader(m_vShaderID);
	if (vShader) vShader->Unbind();

	const ShaderD3D11* pShader = m_resourceManager->GetShader(m_pShaderID);
	if (pShader) pShader->Unbind();

	for (auto cs : m_cShaderIDs)
	{
		const ShaderD3D11* cShader = m_resourceManager->GetShader(cs);
		if (cShader) cShader->Unbind();
	}

	ID3D11SamplerState* nullSampler[1] = { nullptr };
	DeviceManager::Context()->PSSetSamplers(0, 1, nullSampler);

	constexpr ID3D11ShaderResourceView* nullSRVs[3]{ nullptr };
	DeviceManager::Context()->PSSetShaderResources(0, 3, nullSRVs);

	ID3D11UnorderedAccessView* nullUAVs[1] = { nullptr };
	DeviceManager::Context()->CSSetUnorderedAccessViews(0, 1, nullUAVs, nullptr);

	DeviceManager::Context()->CSSetShaderResources(0, 1, nullSRVs);

	ID3D11RenderTargetView* nullRTVs[1] = { nullptr };
	DeviceManager::Context()->OMSetRenderTargets(1, nullRTVs, nullptr);
}

void Pipeline::EntitiesUnsorted()
{
	m_entitiesSorted = false;
}

bool Pipeline::IsValid() const
{
	return (m_inputLayout != nullptr		&&
			m_vShaderID != INVALID_ID		&&
			m_pShaderID != INVALID_ID		&&
			m_rasterizer != nullptr			&&
			m_depthBuffer != nullptr
	);
}

bool Pipeline::IsActive() const
{
	return m_active && IsValid();
}

void Pipeline::Disable()
{
	m_active = false;
}

void Pipeline::Enable()
{
	m_active = true;
}

void Pipeline::ClearRenderTarget(bool on)
{
	m_clearRenderTarget = on;
}

ID3D11ShaderResourceView* Pipeline::GetBackBuffer() const
{
	return m_gBuffer->GetSRV();
}

ID3D11ShaderResourceView* Pipeline::GetDepthBuffer() const
{
	return m_depthBuffer->GetSRV();
}

bool Pipeline::Bind(CameraD3D11* camera)
{
	ZoneScopedC(RandomUniqueColor());
	
	if (!IsValid()) [[unlikely]] 
		return false;

	ID3D11RenderTargetView* rtv = m_rtv->GetRTV();

	// TODO: Move this to a update function?
	m_lightManager.UpdateBuffers();

	// Input Assembler
	DeviceManager::Context()->IASetInputLayout(m_inputLayout->GetInputLayout());

	// Vertex Shader
	const ShaderD3D11* vShader = m_resourceManager->GetShader(m_vShaderID);
	vShader->BindShader();

	// Pixel Shader
	const ShaderD3D11* pShader = m_resourceManager->GetShader(m_pShaderID);
	pShader->BindShader();

	// Compute Shader
	if (HasComputeShader())
		rtv = m_gBuffer->GetRTV();

	// Rasterizer
	DeviceManager::Context()->RSSetState(m_rasterizer->GetState());
	DeviceManager::Context()->RSSetViewports(1, &m_viewport);

	// Output Merger
	ID3D11UnorderedAccessView* nullUAVs[1] = { nullptr };
	DeviceManager::Context()->CSSetUnorderedAccessViews(0, 1, nullUAVs, nullptr);
	DeviceManager::Context()->OMSetRenderTargets(1, &rtv, m_depthBuffer->GetDSV(0));

	// Bind Light Data
	const unsigned int nPSSrvs = 3;
	const unsigned int lightStartSlot = 5;
	ID3D11ShaderResourceView* srvs[nPSSrvs] = { m_lightManager.GetPointLightSRV(),
												m_lightManager.GetSpotLightSRV(), 
												m_lightManager.GetDirectionalLightSRV() };
	DeviceManager::Context()->PSSetShaderResources(lightStartSlot, nPSSrvs, srvs);


	const unsigned int nPSConstBuff = 2;
	ID3D11Buffer* psBuffers[nPSConstBuff] = { camera->GetConstantBuffer(), m_lightManager.GetBuffer() };
	DeviceManager::Context()->PSSetConstantBuffers(0, nPSConstBuff, psBuffers);

	return true;
}

bool Pipeline::BindCompute(ID id)
{	
	if (id != INVALID_ID)
	{
		const ShaderD3D11* cShader = m_resourceManager->GetShader(id);
		cShader->BindShader();

		ID3D11SamplerState* sampler = m_resourceManager->GetSampler(g_defaultSampler)->GetSamplerState();
		DeviceManager::Context()->CSSetSamplers(0, 1, &sampler);
	}

	return true;
}

void Pipeline::BindEntity(const Entity* entity)
{
	// Input Assembler
	DeviceManager::Context()->IASetPrimitiveTopology(entity->GetTopology());

	// Mesh
	ID tempID = entity->GetMesh();
	if (tempID != m_loadedMesh && tempID != INVALID_ID)
	{
		const MeshD3D11* mesh = m_resourceManager->GetMesh(tempID);
		mesh->BindMeshBuffers();
		m_loadedMesh = tempID;
	}

	tempID = entity->GetSampler();
	if (tempID != m_loadedSampler && tempID != INVALID_ID)
	{
		ID3D11SamplerState* samplers[1] = { m_resourceManager->GetSampler(tempID)->GetSamplerState() };
		DeviceManager::Context()->PSSetSamplers(0, 1, samplers);
		m_loadedSampler = tempID;
	}
}

void Pipeline::BindMaterial(const MaterialMaps& material)
{
	BindTexture(0, material.albedoMap);
	BindTexture(1, material.normalMap);
	BindTexture(2, material.ambientOcclusionMap);
	BindTexture(3, material.roughnessMap);
	BindTexture(4, material.metalnessMap);
}

void Pipeline::BindTexture(int slot, ID id)
{
	if (id != m_boundTextures[slot] && id != INVALID_ID)
	{
		const ShaderResourceTextureD3D11* texture = m_resourceManager->GetTexture(id);
		if (!texture)
			texture = g_resourceManager.GetTexture(g_fallbackTex);

		ID3D11ShaderResourceView* psSRVs[1]{ texture->GetSRV() };
		DeviceManager::Context()->PSSetShaderResources(slot, 1, psSRVs);

		m_boundTextures[slot] = id;
	}
	else if (id == INVALID_ID)
	{
		m_boundTextures[slot] = INVALID_ID;
		DeviceManager::Context()->PSSetShaderResources(slot, 1, nullSRV);
	}
}

bool Pipeline::HasComputeShader()
{
	return !std::all_of(m_cShaderIDs.begin(), m_cShaderIDs.end(), [](ID id) { return id == INVALID_ID; });
}


