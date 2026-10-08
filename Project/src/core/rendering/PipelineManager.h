#pragma once

#include <d3d11_4.h>

#include "platform/D3D11Helper.h"

#include "core/rendering/d3d11/resources/ShaderD3D11.h"
#include "core/rendering/d3d11/resources/InputLayoutD3D11.h"
#include "core/rendering/d3d11/resources/RasterizerD3D11.h"
#include "core/rendering/d3d11/resources/SamplerD3D11.h"

#include "core/rendering/d3d11/camera/CameraD3D11.h"

#include "core/rendering/d3d11/buffers/GBufferD3D11.h"

class PipelineManager
{
public:
	static void CreateVertexShader(ShaderD3D11*& vShader);
	static void CreatePixelShader(ShaderD3D11*& pShader);
	static void CreateInputLayout(InputLayoutD3D11*& inputLayout, const ShaderD3D11* vShader);
	static void CreateRasterizer(RasterizerD3D11*& rasterizer, bool wireframeOn = false);
	static void CreateSample(SamplerD3D11*& sampler);
	static void CreateGBuffer(GBufferD3D11*& gBuffer, UINT width, UINT height);

private:
	static std::string m_shaderPath;
	static std::string m_csoPath;
};