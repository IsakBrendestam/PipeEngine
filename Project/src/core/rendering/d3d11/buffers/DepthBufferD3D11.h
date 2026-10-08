#pragma once

#include <d3d11_4.h>

#include <vector>

#include "platform/D3D11Helper.h"

class DepthBufferD3D11
{
private:
	ID3D11Texture2D* m_texture = nullptr;
	std::vector<ID3D11DepthStencilView*> m_depthStencilViews;
	ID3D11ShaderResourceView* m_srv = nullptr;

public:
	DepthBufferD3D11() = default;
	DepthBufferD3D11(UINT width, UINT height, bool hasSRV = false);
	~DepthBufferD3D11();
	DepthBufferD3D11(const DepthBufferD3D11& other) = delete;
	DepthBufferD3D11& operator=(const DepthBufferD3D11& other) = delete;
	DepthBufferD3D11(DepthBufferD3D11&& other) = delete;
	DepthBufferD3D11& operator=(DepthBufferD3D11&& other) = delete;

	void Initialize(UINT width, UINT height,
		bool hasSRV = false, UINT arraySize = 1);

	ID3D11DepthStencilView* GetDSV(UINT arrayIndex) const;
	ID3D11ShaderResourceView* GetSRV() const;
};