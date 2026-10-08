#include "stdafx.h"

#include "RasterizerD3D11.h"

#include "core/rendering/d3d11/DeviceManager.h"

RasterizerD3D11::RasterizerD3D11()
{
	Initialize(false, D3D11_CULL_BACK);
}

RasterizerD3D11::RasterizerD3D11(bool wireframeRendering, D3D11_CULL_MODE cullMode)
{
	Initialize(wireframeRendering, cullMode);
}

RasterizerD3D11::~RasterizerD3D11()
{
	ReleaseCOM(m_state);
}

void RasterizerD3D11::Initialize(bool wireframeRendering, D3D11_CULL_MODE cullMode)
{
	D3D11_RASTERIZER_DESC desc;
	if (wireframeRendering)
		desc.FillMode = D3D11_FILL_WIREFRAME;
	else
		desc.FillMode = D3D11_FILL_SOLID;

	desc.CullMode = cullMode;
	desc.FrontCounterClockwise = false;
	desc.DepthBias = 0;
	desc.DepthBiasClamp = 0;
	desc.SlopeScaledDepthBias = 0;
	desc.DepthClipEnable = true;
	desc.ScissorEnable = false;
	desc.MultisampleEnable = false;
	desc.AntialiasedLineEnable = true;

	HR(DeviceManager::Device()->CreateRasterizerState(&desc, &m_state));
}

ID3D11RasterizerState* RasterizerD3D11::GetState() const
{
	return m_state;
}
