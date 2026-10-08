#pragma once

#include <fstream>
#include <string>
#include <unordered_map>

#include <d3d11_4.h>

#include "platform/D3D11Helper.h"

#include "core/graphics/ShaderMacro.h"

#include "core/rendering/d3d11/buffers/ConstantBufferD3D11.h"

#define MAX_BUFFERS 8

enum class ShaderType
{
	VERTEX_SHADER,
	HULL_SHADER,
	DOMAIN_SHADER,
	GEOMETRY_SHADER,
	PIXEL_SHADER,
	COMPUTE_SHADER,
};

enum class ShaderOptimization
{
	O0,
	O1,
	O2,
	O3,

	OPTIMIZATION_COUNT
};

struct ShaderInformation
{
	struct Resource
	{
		std::string name;
		D3D_SHADER_INPUT_TYPE type;
		UINT bindPoint;
		UINT bindCount;
		UINT flags;
	};

	std::vector<Resource> resources;
};

const static std::unordered_map<D3D_SHADER_INPUT_TYPE, const char*> g_shaderInputTypeStringMap = 
{
	{ D3D_SIT_CBUFFER								, "D3D_SIT_CBUFFER"								},
	{ D3D_SIT_TBUFFER					 			, "D3D_SIT_TBUFFER"								},
	{ D3D_SIT_TEXTURE					 			, "D3D_SIT_TEXTURE"								},
	{ D3D_SIT_SAMPLER					 			, "D3D_SIT_SAMPLER"								},
	{ D3D_SIT_UAV_RWTYPED					 		, "D3D_SIT_UAV_RWTYPED"							},
	{ D3D_SIT_STRUCTURED					 		, "D3D_SIT_STRUCTURED"							},
	{ D3D_SIT_UAV_RWSTRUCTURED						, "D3D_SIT_UAV_RWSTRUCTURED"					},
	{ D3D_SIT_BYTEADDRESS					 		, "D3D_SIT_BYTEADDRESS"							},
	{ D3D_SIT_UAV_RWBYTEADDRESS						, "D3D_SIT_UAV_RWBYTEADDRESS"					},
	{ D3D_SIT_UAV_APPEND_STRUCTURED					, "D3D_SIT_UAV_APPEND_STRUCTURED"				},
	{ D3D_SIT_UAV_CONSUME_STRUCTURED				, "D3D_SIT_UAV_CONSUME_STRUCTURED"				},
	{ D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER			, "D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER"		},
	{ D3D_SIT_RTACCELERATIONSTRUCTURE				, "D3D_SIT_RTACCELERATIONSTRUCTURE"				},
	{ D3D_SIT_UAV_FEEDBACKTEXTURE					, "D3D_SIT_UAV_FEEDBACKTEXTURE"					},
	{ D3D10_SIT_CBUFFER								, "D3D10_SIT_CBUFFER"							},
	{ D3D10_SIT_TBUFFER								, "D3D10_SIT_TBUFFER"							},
	{ D3D10_SIT_TEXTURE								, "D3D10_SIT_TEXTURE"							},
	{ D3D10_SIT_SAMPLER								, "D3D10_SIT_SAMPLER"							},
	{ D3D11_SIT_UAV_RWTYPED							, "D3D11_SIT_UAV_RWTYPED"						},
	{ D3D11_SIT_STRUCTURED							, "D3D11_SIT_STRUCTURED"						},
	{ D3D11_SIT_UAV_RWSTRUCTURED					, "D3D11_SIT_UAV_RWSTRUCTURED"					},
	{ D3D11_SIT_BYTEADDRESS							, "D3D11_SIT_BYTEADDRESS"						},
	{ D3D11_SIT_UAV_RWBYTEADDRESS					, "D3D11_SIT_UAV_RWBYTEADDRESS"					},
	{ D3D11_SIT_UAV_APPEND_STRUCTURED				, "D3D11_SIT_UAV_APPEND_STRUCTURED"				},
	{ D3D11_SIT_UAV_CONSUME_STRUCTURED				, "D3D11_SIT_UAV_CONSUME_STRUCTURED"			},
	{ D3D11_SIT_UAV_RWSTRUCTURED_WITH_COUNTER		, "D3D11_SIT_UAV_RWSTRUCTURED_WITH_COUNTER"		}
};

class ShaderD3D11
{
private:
	ShaderType m_type;

	union
	{
		ID3D11VertexShader* vertex = nullptr;
		ID3D11HullShader* hull;
		ID3D11DomainShader* domain;
		ID3D11GeometryShader* geometry;
		ID3D11PixelShader* pixel;
		ID3D11ComputeShader* compute;
	} m_shader;

	ID3DBlob* m_shaderBlob = nullptr;
	std::unordered_map<std::vector<ShaderMacro>, ID3DBlob*, MacroVectorHash> m_compiledCache;

	std::string m_shaderPath;
	std::string m_shaderByteCode;
	
	ShaderOptimization m_optimizationLevel = ShaderOptimization::O1;

	std::vector<ShaderMacro> m_macroStorage;
	std::vector<D3D_SHADER_MACRO> m_macros;

	ConstantBufferD3D11* m_buffers[MAX_BUFFERS] { nullptr };

public:
	ShaderD3D11() = default;
	~ShaderD3D11();
	ShaderD3D11(ShaderType shaderType, const char* csoPath, const char* shaderPath, ShaderOptimization optimization = ShaderOptimization::O1);
	ShaderD3D11(const ShaderD3D11& other) = delete;
	ShaderD3D11& operator=(const ShaderD3D11& other) = delete;
	ShaderD3D11(ShaderD3D11&& other) = delete;
	ShaderD3D11& operator=(ShaderD3D11&& other) = delete;

	void Initialize(ShaderType shaderType, const char* csoPath, const char* shaderPath, ShaderOptimization optimization = ShaderOptimization::O1);
	void Deconstruct();
	void DeleteShader();

	void Recompile();
	void ForceRecompile();

	const void* GetShaderByteData() const;
	size_t GetShaderByteSize() const;

	ShaderType GetType() const;

	void BindShader() const;
	void BindBuffer(int index) const;
	void Unbind() const;

	void SetOptimization(ShaderOptimization optimization);
	ShaderOptimization GetOptimization() const;

	void SetMacroValue(const char* name, const char* value);
	const std::vector<ShaderMacro>& GetMacros() const;

	bool AddConstantBuffer(int index, void* data, size_t size);
	bool UpdateConstantBuffer(int index, void* data);

	ShaderInformation Reflection() const;

private:
	bool Compile(bool forceCompilation);
	bool CreateShaderBlob(ID3DBlob** shaderBlob);
	bool GetExistingShaderBlob(ID3DBlob** shaderBlob);
	bool ReadShaderFile(std::string& shaderSource) const;
	void CreateShader(ID3DBlob* shaderBlob);

	std::array<ID3D11Buffer*, MAX_BUFFERS> CreateBufferArray(UINT& start, UINT& count) const;
};