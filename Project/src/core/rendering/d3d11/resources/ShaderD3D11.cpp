#include "stdafx.h"
#include <string.h>

#include <d3dcompiler.h>
#include <d3d11shader.h>

#include "ShaderD3D11.h"

#include "core/rendering/d3d11/DeviceManager.h"

using namespace std;

constexpr const char* shaderTarget[6] = {
	"vs_5_0",
	"hs_5_0",
	"ds_5_0",
	"gs_5_0",
	"ps_5_0",
	"cs_5_0"
};

constexpr UINT optimizationFlags[4] = {
	D3DCOMPILE_OPTIMIZATION_LEVEL0,
	D3DCOMPILE_OPTIMIZATION_LEVEL1,
	D3DCOMPILE_OPTIMIZATION_LEVEL2,
	D3DCOMPILE_OPTIMIZATION_LEVEL3
};


ShaderD3D11::~ShaderD3D11()
{
	ZoneScopedC(RandomUniqueColor());
	Deconstruct();
}

ShaderD3D11::ShaderD3D11(ShaderType shaderType, const char* csoPath, const char* shaderPath, ShaderOptimization optimization)
{
	Initialize(shaderType, csoPath, shaderPath, optimization);
}

void ShaderD3D11::Initialize(ShaderType shaderType, const char* csoPath, const char* shaderPath, ShaderOptimization optimization)
{
	ZoneScopedC(RandomUniqueColor());

    for (char c : std::string(shaderPath)) {
        if (c == '\\' || c == '/') {
            m_shaderPath += "\\\\";
        } else {
            m_shaderPath += c;
        }
    }

	m_type = shaderType;
	m_optimizationLevel = optimization;
	m_macros.push_back({ nullptr, nullptr });

	Compile(true);
}

void ShaderD3D11::Deconstruct() 
{
	for (int i = 0; i < MAX_BUFFERS; i++)
		delete m_buffers[i];

	for (auto& shaderBlob : m_compiledCache)
		ReleaseCOM(shaderBlob.second);

	DeleteShader();
}

void ShaderD3D11::DeleteShader()
{
	switch (m_type)
	{
	case ShaderType::VERTEX_SHADER:
		ReleaseCOM(m_shader.vertex);
		break;

	case ShaderType::HULL_SHADER:
		ReleaseCOM(m_shader.hull);
		break;

	case ShaderType::DOMAIN_SHADER:
		ReleaseCOM(m_shader.domain);
		break;

	case ShaderType::GEOMETRY_SHADER:
		ReleaseCOM(m_shader.geometry);
		break;

	case ShaderType::PIXEL_SHADER:
		ReleaseCOM(m_shader.pixel);
		break;

	case ShaderType::COMPUTE_SHADER:
		ReleaseCOM(m_shader.compute)
		break;

	default:
		break;
	}

}

void ShaderD3D11::Recompile()
{
	Compile(false);
}

void ShaderD3D11::ForceRecompile()
{
	Compile(true);
}

const void* ShaderD3D11::GetShaderByteData() const
{
	return m_shaderBlob->GetBufferPointer();
}

size_t ShaderD3D11::GetShaderByteSize() const
{
	return m_shaderBlob->GetBufferSize();
}

ShaderType ShaderD3D11::GetType() const
{
	return m_type;
}

void ShaderD3D11::BindShader() const
{
	ZoneScopedC(RandomUniqueColor());
	UINT start, count;
	auto buffers = CreateBufferArray(start, count);


	switch (m_type)
	{
	case ShaderType::VERTEX_SHADER:
		DeviceManager::Context()->VSSetShader(m_shader.vertex, nullptr, 0);
		DeviceManager::Context()->VSSetConstantBuffers(start, count, buffers.data());
		break;

	case ShaderType::HULL_SHADER:
		DeviceManager::Context()->HSSetShader(m_shader.hull, nullptr, 0);
		DeviceManager::Context()->HSSetConstantBuffers(start, count, buffers.data());
		break;
			
	case ShaderType::DOMAIN_SHADER:
		DeviceManager::Context()->DSSetShader(m_shader.domain, nullptr, 0);
		DeviceManager::Context()->DSSetConstantBuffers(start, count, buffers.data());
		break;

	case ShaderType::GEOMETRY_SHADER:
		DeviceManager::Context()->GSSetShader(m_shader.geometry, nullptr, 0);
		DeviceManager::Context()->GSSetConstantBuffers(start, count, buffers.data());
		break;

	case ShaderType::PIXEL_SHADER:
		DeviceManager::Context()->PSSetShader(m_shader.pixel, nullptr, 0);
		DeviceManager::Context()->PSSetConstantBuffers(start, count, buffers.data());
		break;

	case ShaderType::COMPUTE_SHADER:
		DeviceManager::Context()->CSSetShader(m_shader.compute, nullptr, 0);
		DeviceManager::Context()->CSSetConstantBuffers(start, count, buffers.data());
		break;

	default:
		break;
	}
}

void ShaderD3D11::BindBuffer(int index) const
{
	const unsigned int nVSBuff = 1;
	ID3D11Buffer* vsBuffers[nVSBuff] = { m_buffers[index]->GetBuffer() };
	DeviceManager::Context()->VSSetConstantBuffers(index, nVSBuff, vsBuffers);
}

void ShaderD3D11::Unbind() const
{
	ZoneScopedC(RandomUniqueColor());

	UINT start = 0, count = MAX_BUFFERS;
	std::array<ID3D11Buffer*, MAX_BUFFERS> buffers{ nullptr };

	switch (m_type)
	{
	case ShaderType::VERTEX_SHADER:
		DeviceManager::Context()->VSSetConstantBuffers(start, count, buffers.data());
		DeviceManager::Context()->VSSetShader(nullptr, nullptr, 0);
		break;

	case ShaderType::HULL_SHADER:
		DeviceManager::Context()->HSSetConstantBuffers(start, count, buffers.data());
		DeviceManager::Context()->HSSetShader(nullptr, nullptr, 0);
		break;

	case ShaderType::DOMAIN_SHADER:
		DeviceManager::Context()->DSSetConstantBuffers(start, count, buffers.data());
		DeviceManager::Context()->DSSetShader(nullptr, nullptr, 0);
		break;

	case ShaderType::GEOMETRY_SHADER:
		DeviceManager::Context()->GSSetConstantBuffers(start, count, buffers.data());
		DeviceManager::Context()->GSSetShader(nullptr, nullptr, 0);
		break;

	case ShaderType::PIXEL_SHADER:
		DeviceManager::Context()->PSSetConstantBuffers(start, count, buffers.data());
		DeviceManager::Context()->PSSetShader(nullptr, nullptr, 0);
		break;

	case ShaderType::COMPUTE_SHADER:
		DeviceManager::Context()->CSSetConstantBuffers(start, count, buffers.data());
		DeviceManager::Context()->CSSetShader(nullptr, nullptr, 0);
		break;

	default:
		break;
	}
}

void ShaderD3D11::SetOptimization(ShaderOptimization optimization)
{
	m_optimizationLevel = optimization;
}

ShaderOptimization ShaderD3D11::GetOptimization() const
{
	return m_optimizationLevel;
}

void ShaderD3D11::SetMacroValue(const char* name, const char* value)
{
	const auto it = std::find_if(m_macroStorage.begin(), m_macroStorage.end(), [&name](const ShaderMacro& m) {
		return strcmp(m.name, name) == 0;
	});

	if (it != m_macroStorage.end())
	{
		strncpy_s(it->value, value, 63);
		it->value[63] = '\0';
	}
	else
	{
		ShaderMacro macro;

		strncpy_s(macro.name, name, 63);
		macro.name[63] = '\0';

		strncpy_s(macro.value, value, 63);
		macro.value[63] = '\0';

		m_macroStorage.push_back(macro); 
	}

	// Rebuild macro list
	m_macros.clear();
    for (auto& m : m_macroStorage)
        m_macros.push_back({
            m.name,
            m.value
        });
    m_macros.push_back({ nullptr, nullptr }); // Required null entry at the end
}

const std::vector<ShaderMacro>& ShaderD3D11::GetMacros() const
{
	return m_macroStorage;
}

bool ShaderD3D11::AddConstantBuffer(int index, void* data, size_t size)
{
	ZoneScopedC(RandomUniqueColor());

	if (index >= MAX_BUFFERS) return false;

	m_buffers[index] = new ConstantBufferD3D11(size, true, data);
	return true;
}

bool ShaderD3D11::UpdateConstantBuffer(int index, void* data)
{
	ZoneScopedC(RandomUniqueColor());

	if (index >= MAX_BUFFERS) return false;

	if (!m_buffers[index])
		return false;
	m_buffers[index]->UpdateBuffer(data);

	return true;
}

ShaderInformation ShaderD3D11::Reflection() const
{
	ShaderInformation info;

	if (!m_shaderBlob)
		return info;

	ID3D11ShaderReflection* reflection = nullptr;

	HR(D3DReflect(
		m_shaderBlob->GetBufferPointer(), 
		m_shaderBlob->GetBufferSize(), 
		IID_ID3D11ShaderReflection, 
		(void**)&reflection
	));

	D3D11_SHADER_DESC shaderDesc;
	reflection->GetDesc(&shaderDesc);

	for (UINT i = 0; i < shaderDesc.BoundResources; i++)
	{
		D3D11_SHADER_INPUT_BIND_DESC bindDesc;
		reflection->GetResourceBindingDesc(i, &bindDesc);

		ShaderInformation::Resource res;
		res.name = bindDesc.Name;
		res.type = bindDesc.Type;
		res.bindPoint = bindDesc.BindPoint;
		res.bindCount = bindDesc.BindCount;
		res.flags = bindDesc.uFlags;

		info.resources.push_back(res);
	}

	return info;
}

bool ShaderD3D11::Compile(bool forceCompilation)
{
	ZoneScopedNC("Compile Shader", RandomUniqueColor());
	InternalScope(std::format("Last Compilation (Name: {})", m_shaderPath));

	if (forceCompilation)
		m_compiledCache.clear();

	ID3DBlob* shaderBlob;
	if (forceCompilation || !GetExistingShaderBlob(&shaderBlob))
		if (!CreateShaderBlob(&shaderBlob))
			return false;

	DeleteShader();

	CreateShader(shaderBlob);

	return true;
}

bool ShaderD3D11::CreateShaderBlob(ID3DBlob** shaderBlob)
{
	ZoneScopedNC("Crete shader blob", RandomUniqueColor());
	DebugLog("Compiling shader");

	string shaderSource;
	ReadShaderFile(shaderSource);

	ID3DBlob* error;
	ID3DBlob* shader;

	// Compile shader
	HRESULT hr = D3DCompile(
		shaderSource.c_str(),
		shaderSource.size(),
		m_shaderPath.c_str(),
		m_macros.data(),
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		shaderTarget[(int)m_type],
		optimizationFlags[(int)m_optimizationLevel],
		0,
		&shader,
		&error
	);

	if (FAILED(hr))
	{
		if (error)
		{
			DebugLog("ShaderError: " + string((char*)error->GetBufferPointer()));
			ReleaseCOM(error);
		}
		return false;
	}

	// Only update shader if successfull
	*shaderBlob = shader;

	return true;
}

bool ShaderD3D11::GetExistingShaderBlob(ID3DBlob** shaderBlob)
{
	auto it = m_compiledCache.find(m_macroStorage);
	if (it == m_compiledCache.end())
		return false;
    *shaderBlob = it->second;
	return true;
}

bool ShaderD3D11::ReadShaderFile(string& shaderSource) const
{
	ZoneScopedNC("Read shader file", RandomUniqueColor());

	std::ifstream reader;

	// Open File
	reader.open(m_shaderPath, std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		DebugLog("Failed to open shader file: " + std::string(m_shaderPath));
		return false;
	}

	// Read File Data
	reader.seekg(0, ios::end);
	shaderSource.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);

	shaderSource.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

	return true;
}

void ShaderD3D11::CreateShader(ID3DBlob* shaderBlob)
{	
	ZoneScopedNC("Create Shader", RandomUniqueColor());

	m_shaderBlob = shaderBlob;
	m_compiledCache[m_macroStorage] = shaderBlob;

	switch (m_type)
	{
	case ShaderType::VERTEX_SHADER:
		HR(DeviceManager::Device()->CreateVertexShader(
			m_shaderBlob->GetBufferPointer(), 
			m_shaderBlob->GetBufferSize(), 
			nullptr, 
			&m_shader.vertex
		));
		break;

	case ShaderType::HULL_SHADER:
		HR(DeviceManager::Device()->CreateHullShader(
			m_shaderBlob->GetBufferPointer(), 
			m_shaderBlob->GetBufferSize(), 
			nullptr, 
			&m_shader.hull
		));
		break;

	case ShaderType::DOMAIN_SHADER: 
		HR(DeviceManager::Device()->CreateDomainShader(
			m_shaderBlob->GetBufferPointer(), 
			m_shaderBlob->GetBufferSize(), 
			nullptr, 
			&m_shader.domain
		));
		break;

	case ShaderType::GEOMETRY_SHADER:
		HR(DeviceManager::Device()->CreateGeometryShader(
			m_shaderBlob->GetBufferPointer(), 
			m_shaderBlob->GetBufferSize(), 
			nullptr,
			&m_shader.geometry
		));
		break;

	case ShaderType::PIXEL_SHADER:
		HR(DeviceManager::Device()->CreatePixelShader(
			m_shaderBlob->GetBufferPointer(), 
			m_shaderBlob->GetBufferSize(), 
			nullptr,
			&m_shader.pixel
		));
		break;

	case ShaderType::COMPUTE_SHADER:
		HR(DeviceManager::Device()->CreateComputeShader(
			m_shaderBlob->GetBufferPointer(), 
			m_shaderBlob->GetBufferSize(), 
			nullptr,
			&m_shader.compute
		));
		break;

	default:
		break;
	}

}

std::array<ID3D11Buffer*, MAX_BUFFERS>  ShaderD3D11::CreateBufferArray(UINT& start, UINT& count) const
{
	start = 0; count = 0;
	std::array<ID3D11Buffer*, MAX_BUFFERS> buffers{ nullptr };
	for (int i = 0; i < MAX_BUFFERS; i++)
		if (m_buffers[i])
		{
			if (count == 0) 
				start = i;
			buffers[count] = m_buffers[i]->GetBuffer();
			count++;
		}
	return buffers;
}
