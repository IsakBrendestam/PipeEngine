#pragma once

#include <array>

#include <d3d11_4.h>

#include "platform/D3D11Helper.h"

class SamplerD3D11
{
private:
	ID3D11SamplerState* m_sampler = nullptr;

public:
	SamplerD3D11() = default;
	SamplerD3D11(D3D11_TEXTURE_ADDRESS_MODE adressMode,
		std::array<float, 4> borderColour = { 0, 0, 0, 0 });
	~SamplerD3D11();
	SamplerD3D11(const SamplerD3D11& other) = delete;
	SamplerD3D11& operator=(const SamplerD3D11& other) = delete;
	SamplerD3D11(SamplerD3D11&& other) = delete;
	SamplerD3D11& operator=(SamplerD3D11&& other) = delete;

	void Initialize(D3D11_TEXTURE_ADDRESS_MODE adressMode,
		std::array<float, 4> borderColour = {0, 0, 0, 0});

	ID3D11SamplerState* GetSamplerState() const;
};