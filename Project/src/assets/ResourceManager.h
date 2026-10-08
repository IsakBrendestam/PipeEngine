#pragma once

#include <assimp/scene.h>

#include <unordered_map>

#include "core/UniqueID.h"

#include "core/rendering/d3d11/mesh/MeshD3D11.h"
#include "core/rendering/d3d11/resources/ShaderD3D11.h"
#include "core/rendering/d3d11/resources/ShaderResourceTextureD3D11.h"

#include "core/graphics/Texture2D.h"


class ResourceManager
{
public:
	ResourceManager() = default;
	ResourceManager(const ResourceManager& other);
	~ResourceManager();

	void LoadDefaultContent();

	void Append(const ResourceManager& other);

	std::vector<MaterialMaps> ProcessMaterial(const aiScene* scene);

	ID AddMesh(const std::string& meshPath, const std::string& meshFileName, const std::string& name, bool blenderImport = true);
	const MeshD3D11* GetMesh(ID id);
	const MeshD3D11* GetMesh(const std::string& name);
	ID GetMeshID(const std::string& name);
	const std::unordered_map<std::string, ID>& GetMeshes();

	ID AddTexture(const std::string& texturePath, const std::string& name, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);
	ID AddTexture(const std::string& name, UINT width, UINT height, void* textureData, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, unsigned char forcedChannels = 4);
	const ShaderResourceTextureD3D11* GetTexture(ID id) const;
	const ShaderResourceTextureD3D11* GetTexture(const std::string& name) const;
	ID GetTextureID(const std::string& name);
	std::unordered_map<std::string, ID> GetTextures() const;
	
	ID AddShader(const std::string& shaderFileName, const std::string& name, ShaderType type);
	const ShaderD3D11* GetShader(ID id);
	const ShaderD3D11* GetShader(const std::string& name);
	ShaderD3D11* GetShaderMutable(ID id);
	ID GetShaderID(const std::string& name);
	const std::unordered_map<std::string, ID>& GetShaderIDs();

	ID AddSampler(const std::string& name, D3D11_TEXTURE_ADDRESS_MODE adressMode, std::array<float, 4> borderColour = { 0, 0, 0, 0 });
	const SamplerD3D11* GetSampler(ID id);
	const SamplerD3D11* GetSampler(const std::string& name);
	ID GetSamplerID(const std::string& name);

	void Deallocate();

private:
	std::unordered_map<ID, std::shared_ptr<MeshD3D11>> m_meshes;
	std::unordered_map<std::string, ID> m_meshNames;

	std::unordered_map<ID, std::shared_ptr<ShaderResourceTextureD3D11>> m_textures;
	std::unordered_map<std::string, ID> m_textureNames;

	std::unordered_map<ID, std::shared_ptr<ShaderD3D11>> m_shaders;
	std::unordered_map<std::string, ID> m_shaderNames;

	std::unordered_map<ID, std::shared_ptr<SamplerD3D11>> m_samplers;
	std::unordered_map<std::string, ID> m_samplerNames;
};

extern ResourceManager g_resourceManager;

extern ID g_fallbackTex;
extern ID g_defaultNormalTex;
extern ID g_defaultAmbientOcclusionTex;
extern ID g_defaultSampler;