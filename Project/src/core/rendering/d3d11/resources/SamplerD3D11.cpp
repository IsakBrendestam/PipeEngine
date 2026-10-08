#include "stdafx.h"

#include "SamplerD3D11.h"

#include "core/rendering/d3d11/DeviceManager.h"

SamplerD3D11::SamplerD3D11(D3D11_TEXTURE_ADDRESS_MODE adressMode, std::array<float, 4> borderColour)
{
	Initialize(adressMode, borderColour);
}

SamplerD3D11::~SamplerD3D11()
{
	ReleaseCOM(m_sampler);
}

void SamplerD3D11::Initialize(D3D11_TEXTURE_ADDRESS_MODE adressMode, std::array<float, 4> borderColour)
{
	D3D11_SAMPLER_DESC sampler;
	sampler.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler.AddressU = adressMode;
	sampler.AddressV = adressMode;
	sampler.AddressW = adressMode;
	sampler.MipLODBias = 0;
	sampler.MaxAnisotropy = 0;
	sampler.ComparisonFunc = D3D11_COMPARISON_NEVER;
	for (int i = 0; i < 4; i++)
			sampler.BorderColor[i] = borderColour[i];
	sampler.MinLOD = 0;
	sampler.MaxLOD = D3D11_FLOAT32_MAX;

	DeviceManager::Device()->CreateSamplerState(&sampler, &m_sampler);
}

ID3D11SamplerState* SamplerD3D11::GetSamplerState() const
{
	return m_sampler;
}
