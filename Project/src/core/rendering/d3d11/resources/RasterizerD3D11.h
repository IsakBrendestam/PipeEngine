#pragma once

#include "platform/D3D11Helper.h"
#include <d3d11_4.h>

class RasterizerD3D11
{
public:
	RasterizerD3D11();
	RasterizerD3D11(bool wireframeRendering, D3D11_CULL_MODE cullMode = D3D11_CULL_BACK);
	~RasterizerD3D11();
	RasterizerD3D11(const RasterizerD3D11& other) = delete;
	RasterizerD3D11& operator=(const RasterizerD3D11& other) = delete;
	RasterizerD3D11(RasterizerD3D11&& other) noexcept = delete; // Move constructor
	RasterizerD3D11& operator=(RasterizerD3D11&& other) noexcept = delete; // Move assignment operator

	void Initialize(bool wireframeRendering = false, D3D11_CULL_MODE cullMode = D3D11_CULL_BACK);

	ID3D11RasterizerState* GetState() const;

private:
	ID3D11RasterizerState* m_state;
};