#pragma once

#include "platform/D3D11Helper.h"
#include <d3d11_4.h>

class RenderTargetD3D11
{
public:
	RenderTargetD3D11() = default;
	RenderTargetD3D11(UINT width, UINT height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, bool createSRV = true, bool createUAV = true);
	RenderTargetD3D11(IDXGISwapChain* swapChain);
	~RenderTargetD3D11();
	RenderTargetD3D11(const RenderTargetD3D11& other) = delete;
	RenderTargetD3D11& operator=(const RenderTargetD3D11& other) = delete;
	RenderTargetD3D11(RenderTargetD3D11&& other) = delete;
	RenderTargetD3D11& operator=(RenderTargetD3D11&& other) = delete;

	void Initialize(UINT width, UINT height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, bool createSRV = true, bool createUAV = true);
	void Initialize(IDXGISwapChain* swapChain);
	void Resize(UINT width, UINT height);

	void Copy(RenderTargetD3D11* other);

	ID3D11RenderTargetView* GetRTV() const;
	ID3D11UnorderedAccessView* GetUAV() const;
	ID3D11ShaderResourceView* GetSRV() const;

	UINT GetWidth() const { return m_width; }
	UINT GetHeight() const { return m_height; }

private:
	void Release();

private:
	ID3D11Texture2D* m_texture = nullptr;
	ID3D11RenderTargetView* m_rtv = nullptr;
	ID3D11ShaderResourceView* m_srv = nullptr;
	ID3D11UnorderedAccessView* m_uav = nullptr;

	UINT m_width = 0, m_height = 0;
	DXGI_FORMAT m_format = DXGI_FORMAT_R8G8B8A8_UNORM;
	bool m_hasSRV = true;
	bool m_hasUAV = true;
};