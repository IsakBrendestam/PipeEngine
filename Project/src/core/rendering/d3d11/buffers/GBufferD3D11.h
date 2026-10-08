#pragma once

#include "platform/D3D11Helper.h"
#include <d3d11_4.h>

class GBufferD3D11
{
private:
	ID3D11Texture2D* m_texture = nullptr;
	ID3D11ShaderResourceView* m_srv = nullptr;
	ID3D11RenderTargetView* m_rtv = nullptr;

	UINT m_width, m_height;

public:
	GBufferD3D11() = default;
	GBufferD3D11(UINT width, UINT height);
	~GBufferD3D11();
	GBufferD3D11(const GBufferD3D11& other) = delete;
	GBufferD3D11& operator=(const GBufferD3D11& other) = delete;
	GBufferD3D11(GBufferD3D11&& other) = delete;
	GBufferD3D11& operator=(GBufferD3D11&& other) = delete;

	void Initialize(UINT width, UINT height);

	void Clear(const float clearColor[4]);

	ID3D11ShaderResourceView* GetSRV() const;
	ID3D11RenderTargetView* GetRTV() const;

	UINT GetWidth() const;
	UINT GetHeight() const;
};