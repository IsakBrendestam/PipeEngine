#include "stdafx.h"
#include "ResourceManager.h"

#include "assets/parser/ObjParser.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <format>

ResourceManager g_resourceManager;

ID g_fallbackTex = INVALID_ID;
ID g_defaultNormalTex = INVALID_ID;
ID g_defaultAmbientOcclusionTex = INVALID_ID;
ID g_defaultSampler = INVALID_ID;

ResourceManager::ResourceManager(const ResourceManager& other):
    m_meshes(other.m_meshes),       m_meshNames(other.m_meshNames),
    m_textures(other.m_textures),   m_textureNames(other.m_textureNames),
    m_shaders(other.m_shaders),     m_shaderNames(other.m_shaderNames),
    m_samplers(other.m_samplers),   m_samplerNames(other.m_samplerNames)
{
}

ResourceManager::~ResourceManager()
{
    Deallocate();
}

void ResourceManager::LoadDefaultContent()
{
    DebugLog("Loading Deafult Content");

    uint8_t data = 255;
    g_defaultAmbientOcclusionTex = AddTexture("Default_AO", 1, 1, &data, DXGI_FORMAT_R8_UNORM, 1);

    std::array<uint8_t, 4> texData = { 128, 128, 255, 255};
    g_defaultNormalTex = AddTexture("Default_Normal", 1, 1, texData.data(), DXGI_FORMAT_R8G8B8A8_UNORM, 4);

    g_fallbackTex = AddTexture("res/Textures/Fallback.png", "Fallback", DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);

    g_defaultSampler = AddSampler("DefaultSampler", D3D11_TEXTURE_ADDRESS_CLAMP);

	if (DEBUG)
	{
		AddShader("DebugNormal_PS", "DebugNormal_PS", ShaderType::PIXEL_SHADER);
		AddShader("DebugAlbedo_PS", "DebugAlbedo_PS", ShaderType::PIXEL_SHADER);
		AddShader("DebugAmbientOcclusion_PS", "DebugAmbientOcclusion_PS", ShaderType::PIXEL_SHADER);
		AddShader("DebugRoughness_PS", "DebugRoughness_PS", ShaderType::PIXEL_SHADER);
		AddShader("DebugMetalness_PS", "DebugMetalness_PS", ShaderType::PIXEL_SHADER);
		AddShader("DebugPosition_PS", "DebugPosition_PS", ShaderType::PIXEL_SHADER);
		AddShader("DebugWorldPosition_PS", "DebugWorldPosition_PS", ShaderType::PIXEL_SHADER);

		AddShader("Test_PS", "Test_PS", ShaderType::PIXEL_SHADER);
	}
}

void ResourceManager::Append(const ResourceManager& other)
{
    m_meshes.reserve(m_meshes.size() + other.m_meshes.size());
    m_meshes.insert(other.m_meshes.begin(), other.m_meshes.end());

    m_meshNames.reserve(m_meshNames.size() + other.m_meshNames.size());
    m_meshNames.insert(other.m_meshNames.begin(), other.m_meshNames.end());

    m_textures.reserve(m_textures.size() + other.m_textures.size());
    m_textures.insert(other.m_textures.begin(), other.m_textures.end());

    m_textureNames.reserve(m_textureNames.size() + other.m_textureNames.size());
    m_textureNames.insert(other.m_textureNames.begin(), other.m_textureNames.end());

    m_shaders.reserve(m_shaders.size() + other.m_shaders.size());
    m_shaders.insert(other.m_shaders.begin(), other.m_shaders.end());

    m_shaderNames.reserve(m_shaderNames.size() + other.m_shaderNames.size());
    m_shaderNames.insert(other.m_shaderNames.begin(), other.m_shaderNames.end());

    m_samplers.reserve(m_samplers.size() + other.m_samplers.size());
    m_samplers.insert(other.m_samplers.begin(), other.m_samplers.end());

    m_samplerNames.reserve(m_samplerNames.size() + other.m_samplerNames.size());
    m_samplerNames.insert(other.m_samplerNames.begin(), other.m_samplerNames.end());
}

void ResourceManager::Deallocate()
{
    // Delete meshes
    m_meshes.clear();
    m_meshNames.clear();

    // Delete Shaders
    m_shaders.clear();
    m_shaderNames.clear();

    // Delete Samplers
    m_samplers.clear();
    m_samplerNames.clear();

    // Delete Textures
    m_textures.clear();
    m_textureNames.clear();
}

constexpr unsigned int nTextureTypes = 20;

constexpr std::array<aiTextureType, nTextureTypes> textureTypes = {
	aiTextureType_AMBIENT,
	aiTextureType_AMBIENT_OCCLUSION,
	aiTextureType_BASE_COLOR,
	aiTextureType_CLEARCOAT,
	aiTextureType_DIFFUSE,
	aiTextureType_DIFFUSE_ROUGHNESS,
	aiTextureType_DISPLACEMENT,
	aiTextureType_EMISSION_COLOR,
	aiTextureType_EMISSIVE,
	aiTextureType_HEIGHT,
	aiTextureType_LIGHTMAP,
	aiTextureType_METALNESS,
	aiTextureType_NORMALS,
	aiTextureType_NORMAL_CAMERA,
	aiTextureType_OPACITY,
	aiTextureType_REFLECTION,
	aiTextureType_SHEEN,
	aiTextureType_SHININESS,
	aiTextureType_SPECULAR,
	aiTextureType_TRANSMISSION
};

const std::unordered_map<aiTextureType, size_t> textureTypesIndexMapping = {
    { aiTextureType_AMBIENT,                0   },
    { aiTextureType_AMBIENT_OCCLUSION,      1   },
	{ aiTextureType_BASE_COLOR,             2   },
	{ aiTextureType_CLEARCOAT,              3   },
	{ aiTextureType_DIFFUSE,                4   },
	{ aiTextureType_DIFFUSE_ROUGHNESS,      5   },
	{ aiTextureType_DISPLACEMENT,           6   },
	{ aiTextureType_EMISSION_COLOR,         7   },
	{ aiTextureType_EMISSIVE,               8   },
	{ aiTextureType_HEIGHT,                 9   },
	{ aiTextureType_LIGHTMAP,               10  },
	{ aiTextureType_METALNESS,              11  },
	{ aiTextureType_NORMALS,                12  },
	{ aiTextureType_NORMAL_CAMERA,          13  },
	{ aiTextureType_OPACITY,                14  },
	{ aiTextureType_REFLECTION,             15  },
	{ aiTextureType_SHEEN,                  16  },
	{ aiTextureType_SHININESS,              17  },
	{ aiTextureType_SPECULAR,               18  },
	{ aiTextureType_TRANSMISSION,           19  }
};

// TODO: Replace with function that considers bit counts
const std::unordered_map<aiTextureType, DXGI_FORMAT> textureTypesFormatMapping = {
    { aiTextureType_AMBIENT,                DXGI_FORMAT_R8G8B8A8_UNORM_SRGB     },
    { aiTextureType_AMBIENT_OCCLUSION,      DXGI_FORMAT_R8G8B8A8_UNORM          },
	{ aiTextureType_BASE_COLOR,             DXGI_FORMAT_R8G8B8A8_UNORM_SRGB     },
	{ aiTextureType_CLEARCOAT,              DXGI_FORMAT_R8G8B8A8_UNORM          },
	{ aiTextureType_DIFFUSE,                DXGI_FORMAT_R8G8B8A8_UNORM_SRGB     },
	{ aiTextureType_DIFFUSE_ROUGHNESS,      DXGI_FORMAT_R8G8B8A8_UNORM          },
	{ aiTextureType_DISPLACEMENT,           DXGI_FORMAT_R8G8B8A8_UNORM          },
	{ aiTextureType_EMISSION_COLOR,         DXGI_FORMAT_R8G8B8A8_UNORM_SRGB     },
	{ aiTextureType_EMISSIVE,               DXGI_FORMAT_R8G8B8A8_UNORM_SRGB     },
	{ aiTextureType_HEIGHT,                 DXGI_FORMAT_R8G8B8A8_UNORM          },
	{ aiTextureType_LIGHTMAP,               DXGI_FORMAT_R8G8B8A8_UNORM_SRGB     }, // Should be SRGB if baked ligthing, UNORM if directional/lighting coefficients
	{ aiTextureType_METALNESS,              DXGI_FORMAT_R8G8B8A8_UNORM          },
	{ aiTextureType_NORMALS,                DXGI_FORMAT_R8G8B8A8_UNORM          },
	{ aiTextureType_NORMAL_CAMERA,          DXGI_FORMAT_R8G8B8A8_UNORM          },
	{ aiTextureType_OPACITY,                DXGI_FORMAT_R8G8B8A8_UNORM          },
	{ aiTextureType_REFLECTION,             DXGI_FORMAT_R8G8B8A8_UNORM_SRGB     },
	{ aiTextureType_SHEEN,                  DXGI_FORMAT_R8G8B8A8_UNORM          },
	{ aiTextureType_SHININESS,              DXGI_FORMAT_R8G8B8A8_UNORM          },
	{ aiTextureType_SPECULAR,               DXGI_FORMAT_R8G8B8A8_UNORM_SRGB     },
	{ aiTextureType_TRANSMISSION,           DXGI_FORMAT_R8G8B8A8_UNORM          }
};

constexpr std::array<const char*, nTextureTypes> textureTypeNames = {
	"Ambient", 
	"Ambient_occlusion",
	"Base_color",
	"Clearcoat",
	"Diffuse",
	"Diffuse_roughness",
	"Displacement",
	"Emission_color",
	"Emissive",
	"Height",
	"Lightmap",
	"Metalness",
	"Normals",
	"Normal_camera",
	"Opacity",
	"Reflection",
	"Sheen",
	"Shininess",
	"Specular",
	"Transmission"
};


std::vector<MaterialMaps> ResourceManager::ProcessMaterial(const aiScene* scene)
{
	ZoneScopedC(RandomUniqueColor());

	std::vector<MaterialMaps> materials;

    DebugLog("----- " + std::string(scene->mName.C_Str()) + " -----");

	for (size_t m = 0; m < scene->mNumMaterials; m++)
	{
		aiMaterial* mat = scene->mMaterials[m];
        std::string matName = std::string(mat->GetName().C_Str());
        ZoneScopedNCD(std::string("Process Material: " + matName).c_str(), RandomUniqueColor());

        std::array<ID, nTextureTypes> textureIDs { INVALID_ID };

		for (unsigned int i = 0; i < nTextureTypes; i++)
		{
		    aiString path;
            aiTextureType textureType = textureTypes[i];
			if (mat->GetTexture(textureType, 0, &path) != aiReturn_SUCCESS)
				continue;

            ZoneScopedNCD(std::string("Process Texture: " + std::string(path.C_Str())).c_str(), RandomUniqueColor());

			std::string texturePath = path.C_Str();
			std::string textureName = matName + "_" + textureTypeNames[i];

            ID tempID = INVALID_ID;

			if (texturePath[0] == '*')
			{
				// Embedded texture
				const aiTexture* texture = scene->GetEmbeddedTexture(texturePath.c_str());

				if (texture->mHeight == 0)
				{
					// Compressed texture
					const stbi_uc* data = reinterpret_cast<const stbi_uc*>(texture->pcData);
					int width, height, channels;
					const stbi_uc* loadedData = stbi_load_from_memory(
						data,
						static_cast<int>(texture->mWidth),
						&width,
						&height,
						&channels,
						4
					);

					if (!loadedData)
						continue;

                    tempID = AddTexture(
                        textureName,
                        width, 
                        height, 
                        (void*)loadedData, 
                        textureTypesFormatMapping.at(textureType),
                        channels
                    );

                    stbi_image_free((void*)loadedData);
				}
				else
				{
					// Uncompressed texture
                    tempID = AddTexture(
                        textureName,
                        texture->mWidth,
                        texture->mHeight,
                        (void*)texture->pcData,
                        textureTypesFormatMapping.at(textureType),
                        4
                    );
				}
			}
			else
			{
				// External texture
                tempID = AddTexture(
                    texturePath,
                    textureName,
                    textureTypesFormatMapping.at(textureType)
                );
			}

            textureIDs[i] = tempID;
		}

		materials.emplace_back(
            textureIDs[textureTypesIndexMapping.at(aiTextureType_BASE_COLOR)],
            textureIDs[textureTypesIndexMapping.at(aiTextureType_NORMALS)],
            textureIDs[textureTypesIndexMapping.at(aiTextureType_AMBIENT_OCCLUSION)],
            textureIDs[textureTypesIndexMapping.at(aiTextureType_DIFFUSE_ROUGHNESS)],
            textureIDs[textureTypesIndexMapping.at(aiTextureType_METALNESS)]
        );
	}	

	return materials;
}

ID ResourceManager::AddMesh(const std::string& meshPath, const std::string& meshFileName, const std::string& name, bool blenderImport)
{
    ZoneScopedNCD(std::string("Add Mesh " + name).c_str(), RandomUniqueColor());

    if (m_meshNames.count(name) != 0)
        return m_meshNames[name];
    
    // -- Load mesh using assimp --
    
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(meshPath + meshFileName, 
        aiProcess_Triangulate |
        aiProcess_CalcTangentSpace |
        aiProcess_ImproveCacheLocality |
        aiProcess_JoinIdenticalVertices |
        aiProcess_FlipUVs |
        aiProcess_ConvertToLeftHanded |
        aiProcess_SortByPType);

    if (!scene || !scene->HasMeshes())
        return INVALID_ID;

    MeshData meshData;
    std::vector<ShaderVertex> vertices;
    std::vector<uint32_t> indices;

    DX::XMFLOAT3 bbMin(FLT_MAX, FLT_MAX, FLT_MAX);
    DX::XMFLOAT3 bbMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    size_t globalIndexOffset = 0;

    std::vector<MaterialMaps> materials = ProcessMaterial(scene);

    for (unsigned int m = 0; m < scene->mNumMeshes; m++)
    {
        aiMesh* mesh = scene->mMeshes[m];

        ZoneScopedNCD(std::string("Process Sub-Mesh: " + std::string(mesh->mName.C_Str())).c_str(), RandomUniqueColor());

        // Process Vertices
		size_t vertexBasedIndex = vertices.size();

        {
            ZoneScopedNCD(std::string("Process Vertices").c_str(), RandomUniqueColor());

			for (unsigned int v = 0; v < mesh->mNumVertices; v++)
			{
				ShaderVertex vertex {};

				aiVector3D pos = mesh->mVertices[v];
				aiVector3D norm = mesh->HasNormals() ? mesh->mNormals[v] : aiVector3D(0);
				aiVector3D tan = mesh->HasTangentsAndBitangents() ? mesh->mTangents[v] : aiVector3D(0);
				aiVector3D bitan = mesh->HasTangentsAndBitangents() ? mesh->mBitangents[v] : aiVector3D(0);
				aiVector3D uv = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][v] : aiVector3D(0);

				vertex.position = DX::XMFLOAT3(pos.x, pos.y, pos.z);
				vertex.normal = DX::XMFLOAT3(norm.x, norm.y, norm.z);
				vertex.tangent = DX::XMFLOAT3(tan.x, tan.y, tan.z);
				vertex.bitangent = DX::XMFLOAT3(bitan.x, bitan.y, bitan.z);
				vertex.uv = DX::XMFLOAT2(uv.x, uv.y);

				bbMin.x = std::min(bbMin.x, pos.x);
				bbMin.y = std::min(bbMin.y, pos.y);
				bbMin.z = std::min(bbMin.z, pos.z);

				bbMax.x = std::max(bbMax.x, pos.x);
				bbMax.y = std::max(bbMax.y, pos.y);
				bbMax.z = std::max(bbMax.z, pos.z);

				vertices.push_back(vertex);
			}

        }

        // Process indices
        size_t indexCountBefore = indices.size();

        {
            ZoneScopedNCD(std::string("Process Indices").c_str(), RandomUniqueColor());

            for (unsigned int f = 0; f < mesh->mNumFaces; f++)
            {
                aiFace& face = mesh->mFaces[f];
                if (face.mNumIndices != 3)
                    continue;

                for (unsigned int i = 0; i < 3; i++)
                    indices.push_back(vertexBasedIndex + face.mIndices[i]);
            }
        }

        // Process sub-mesh
        MeshData::SubMeshInfo subMesh{};
        subMesh.startIndexValue = globalIndexOffset;
        subMesh.nrOfIndicesInSubMesh = indices.size() - indexCountBefore;

        subMesh.materialMaps = materials[mesh->mMaterialIndex];

        meshData.subMeshInfo.push_back(subMesh);
        globalIndexOffset = indices.size();
    }

    // Load final mesh data
    meshData.vertexInfo.sizeOfVertex = sizeof(ShaderVertex);
    meshData.vertexInfo.nrOfVerticesInBuffer = vertices.size();
    meshData.vertexInfo.vertexData = vertices.data();

    meshData.indexInfo.nrOfIndicesInBuffer = indices.size();
    meshData.indexInfo.indexData = indices.data();

    meshData.BBMin = bbMin;
    meshData.BBMax = bbMax;

    // Craete mesh
    ID id = UniqueID::GenerateID();
    m_meshNames[name] = id;

    m_meshes[id] = std::make_shared<MeshD3D11>(meshData);

    return id;
}

const MeshD3D11* ResourceManager::GetMesh(ID id)
{
    return m_meshes[id].get();
}

const MeshD3D11* ResourceManager::GetMesh(const std::string& name)
{
    return m_meshes[m_meshNames[name]].get();
}

ID ResourceManager::GetMeshID(const std::string& name)
{
    return m_meshNames[name];
}

const std::unordered_map<std::string, ID>& ResourceManager::GetMeshes()
{
    return m_meshNames;
}


ID ResourceManager::AddTexture(const std::string& texturePath, const std::string& name, DXGI_FORMAT format)
{
    ZoneScopedNCD(std::string("Add Texture " + name).c_str(), RandomUniqueColor());

    if (m_textureNames.count(name) != 0)
        return m_textureNames[name];

    ID id = UniqueID::GenerateID();
    m_textureNames[name] = id;
    m_textures[id] = std::make_shared<ShaderResourceTextureD3D11>(texturePath.c_str(), format);

    return id;
}

ID ResourceManager::AddTexture(const std::string& name, UINT width, UINT height, void* textureData, DXGI_FORMAT format, unsigned char forcedChannels)
{
    ZoneScopedNCD(std::string("Add Texture " + name).c_str(), RandomUniqueColor());

    if (m_textureNames.count(name) != 0)
        return m_textureNames[name];

    ID id = UniqueID::GenerateID();
    m_textureNames[name] = id;
    m_textures[id] = std::make_shared<ShaderResourceTextureD3D11>(width, height, textureData, format, forcedChannels);

    return id;
}

const ShaderResourceTextureD3D11* ResourceManager::GetTexture(ID id)  const
{
    return m_textures.at(id).get();
}

const ShaderResourceTextureD3D11* ResourceManager::GetTexture(const std::string& name) const
{
    return m_textures.at(m_textureNames.at(name)).get();
}

ID ResourceManager::GetTextureID(const std::string& name)
{
    return m_textureNames[name];
}

std::unordered_map<std::string, ID> ResourceManager::GetTextures() const 
{
    return m_textureNames;
}


ID ResourceManager::AddShader(const std::string& shaderFileName, const std::string& name, ShaderType type)
{
    ZoneScopedNCD(std::string("Add Shader " + name).c_str(), RandomUniqueColor());

    if (m_shaderNames.count(name) != 0)
        return m_shaderNames[name];

    ID id = UniqueID::GenerateID();
    m_shaderNames[name] = id;

	const std::string shaderPath = std::format("{}\\{}.{}", SHADER_PATH, shaderFileName, SHADER_EXTENSION);
	const std::string csoPath = std::format("{}\\{}.cso", CSO_PATH, shaderFileName);
    m_shaders[id] = std::make_shared<ShaderD3D11>(type, csoPath.c_str(), shaderPath.c_str());

    return id;
}

const ShaderD3D11* ResourceManager::GetShader(ID id)
{
    return m_shaders[id].get();
}

const ShaderD3D11* ResourceManager::GetShader(const std::string& name)
{
    return m_shaders[m_shaderNames[name]].get();
}

ShaderD3D11* ResourceManager::GetShaderMutable(ID id)
{
    return m_shaders[id].get();
}

ID ResourceManager::GetShaderID(const std::string& name)
{
    return m_shaderNames[name];
}

const std::unordered_map<std::string, ID>& ResourceManager::GetShaderIDs()
{
    return m_shaderNames;
}


ID ResourceManager::AddSampler(const std::string& name, D3D11_TEXTURE_ADDRESS_MODE adressMode, std::array<float, 4> borderColour)
{
    ZoneScopedNCD(std::string("Add Sampler " + name).c_str(), RandomUniqueColor());

    if (m_samplerNames.count(name) != 0)
        return m_samplerNames[name];

    ID id = UniqueID::GenerateID();
    m_samplerNames[name] = id;
    m_samplers[id] = std::make_shared<SamplerD3D11>(adressMode, borderColour);

    return id;
}

const SamplerD3D11* ResourceManager::GetSampler(ID id)
{
    return m_samplers[id].get();
}

const SamplerD3D11* ResourceManager::GetSampler(const std::string& name)
{
    return m_samplers[m_samplerNames[name]].get();
}

ID ResourceManager::GetSamplerID(const std::string& name)
{
    return m_samplerNames[name];
}

