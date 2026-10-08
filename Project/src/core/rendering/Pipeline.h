#pragma once

#include "core/rendering/d3d11/buffers/DepthBufferD3D11.h"
#include "core/rendering/d3d11/buffers/ConstantBufferD3D11.h"

#include "core/rendering/d3d11/resources/RasterizerD3D11.h"
#include "core/rendering/d3d11/resources/InputLayoutD3D11.h"
#include "core/rendering/d3d11/resources/ShaderD3D11.h"
#include "core/rendering/d3d11/resources/RenderTargetD3D11.h"

#include "core/rendering/d3d11/camera/CameraD3D11.h"

#include "core/graphics/Light.h"

#include "systems/LightManager.h"

#include "core/scene/Entity.h"

#include "assets/ResourceManager.h"

#include <vector>

#define MAX_BUFFERS 8
#define MAX_TEXTURES 8

class Scene;
class PipelineUI;

class Pipeline
{
	friend class PipelineUI;

public:
	Pipeline();
	~Pipeline();

	void Initialize();

	bool SetShader(ID id, ShaderType type);
	bool RemoveShader(ID id, ShaderType type);
	void DeleteComputeShader(ID id);

	bool AddConstantBuffer(int index, void* data, size_t size, ID id, ResourceManager& resourceManager);
	bool UpdateConstantBuffer(int index, void* data, ID id, ResourceManager& resourceManager);

	bool AddTexture(ID3D11ShaderResourceView* texture, ShaderType type, unsigned int index, unsigned int shaderIndex = 0);
	bool SetOutTexture(ID3D11UnorderedAccessView* texture, ShaderType type, unsigned int index, unsigned int shaderIndex = 0);

	void SetViewport(D3D11_VIEWPORT viewport);
	void SetRasterizer(RasterizerD3D11* rasterizer);
	void SetRenderTarget(const RenderTargetD3D11* rtv);

	void AddInputLayout(const std::pair<const char*, DXGI_FORMAT> inputElements[], unsigned int count, ResourceManager& resourceManager);

	void AddEntity(Entity* entity);
	void RemoveEntity(Entity* entity);
	bool HasEntity(const Entity* entity) const;

	void AddLight(PointLight light);
	void AddLight(SpotLight light);
	void AddLight(DirectionalLight light);

	void SetGlobalLighAmbiance(float ambiance);

	const LightManager& GetLightManager() const;

	void SetClearColor(const float clearColor[4]);
	void SetClearColor(const std::array<float, 4> clearColor);

	void ClearRtv();

	bool StartRender(ResourceManager* resourceManager);
	bool Render(CameraD3D11* camera);
	bool RenderEntity(CameraD3D11* camera, Entity* entity);
	bool ComputePass();
	bool EndRender();

	void Unbind();

	void EntitiesUnsorted();

	[[nodiscard]] bool IsValid() const;

	[[nodiscard]] bool IsActive() const;
	void Disable();
	void Enable();

	void ClearRenderTarget(bool on);

	ID3D11ShaderResourceView* GetBackBuffer() const;
	ID3D11ShaderResourceView* GetDepthBuffer() const;

private:
	bool Bind(CameraD3D11* camera);
	bool BindCompute(ID id);
	
	void BindEntity(const Entity* entity);
	void BindMaterial(const MaterialMaps& material);
	void BindTexture(int slot, ID id);

	bool HasComputeShader();

private:
	bool m_active = true;
	bool m_clearRenderTarget = true;

	ResourceManager* m_resourceManager = nullptr;

	const RenderTargetD3D11* m_rtv = nullptr;
	ID3D11UnorderedAccessView* m_uav = nullptr;

	D3D11_VIEWPORT m_viewport;

	DepthBufferD3D11* m_depthBuffer = nullptr;
	RasterizerD3D11* m_rasterizer = nullptr;

	InputLayoutD3D11* m_inputLayout = nullptr;
	std::vector<std::array<ID3D11ShaderResourceView*, MAX_TEXTURES>> m_cTextures;
	std::vector<std::array<ID3D11UnorderedAccessView*, MAX_TEXTURES>> m_cOutTextures;

	ID m_vShaderID = INVALID_ID;
	ID m_pShaderID = INVALID_ID;
	std::vector<ID> m_cShaderIDs;

	LightManager m_lightManager;

	GBufferD3D11* m_gBuffer = nullptr;

	D3D11_PRIMITIVE_TOPOLOGY m_topology;

	bool m_entitiesSorted = false;
	std::vector<Entity*> m_entities;

	static const char m_textureCount = 16;
	ID m_boundTextures[m_textureCount] { INVALID_ID };

	ID m_loadedMesh = INVALID_ID, 
	   m_loadedSampler = INVALID_ID;

	float m_clearColor[4] {0};
};
