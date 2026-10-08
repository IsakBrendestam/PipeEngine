#include "stdafx.h"

#include "PipelineManager.h"

std::string PipelineManager::m_shaderPath = SHADER_PATH;
std::string PipelineManager::m_csoPath = CSO_PATH;

void PipelineManager::CreateVertexShader(ShaderD3D11*& vShader)
{
	const char* shaderName = "TextureVertexShader";
	const std::string shaderPath = std::format("{}\\{}.{}", SHADER_PATH, shaderName, SHADER_EXTENSION);
	const std::string csoPath = std::format("{}\\{}.cso", CSO_PATH, shaderName);
	vShader = new ShaderD3D11(ShaderType::VERTEX_SHADER, csoPath.c_str(), shaderPath.c_str());
}

void PipelineManager::CreatePixelShader(ShaderD3D11*& pShader)
{
	const char* shaderName = "TexturePixelShader";
	const std::string shaderPath = std::format("{}\\{}.{}", SHADER_PATH, shaderName, SHADER_EXTENSION);
	const std::string csoPath = std::format("{}\\{}.cso", CSO_PATH, shaderName);
	pShader = new ShaderD3D11(ShaderType::PIXEL_SHADER, csoPath.c_str(), shaderPath.c_str());
}

void PipelineManager::CreateInputLayout(InputLayoutD3D11*& inputLayout, const ShaderD3D11* vShader)
{
	inputLayout = new InputLayoutD3D11();
	inputLayout->AddInputElement("POSITION", DXGI_FORMAT_R32G32B32_FLOAT);
	inputLayout->AddInputElement("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT);
	inputLayout->AddInputElement("UV", DXGI_FORMAT_R32G32_FLOAT);

	inputLayout->FinalizeInputLayout(vShader->GetShaderByteData(), vShader->GetShaderByteSize());
}

void PipelineManager::CreateRasterizer(RasterizerD3D11*& rasterizer, bool wireframeOn)
{
	rasterizer = new RasterizerD3D11(wireframeOn);
}

void PipelineManager::CreateSample(SamplerD3D11*& sampler)
{
	sampler = new SamplerD3D11(D3D11_TEXTURE_ADDRESS_WRAP);
}

void PipelineManager::CreateGBuffer(GBufferD3D11*& gBuffer, UINT width, UINT height)
{
	gBuffer = new GBufferD3D11(width, height);
}
