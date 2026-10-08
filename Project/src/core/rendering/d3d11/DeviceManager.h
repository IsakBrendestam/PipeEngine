#pragma once

#include <d3d11_4.h>

#include "platform/D3D11Helper.h"
#include "resources/RenderTargetD3D11.h"

class DeviceManager
{
public:
	static int Initialize(HWND window, UINT clientWidth, UINT clientHeight);
	static void Deconstruct();
	static void Resize();

	static ID3D11Device* Device();
	static ID3D11DeviceContext* Context();
	static IDXGISwapChain* SwapChain();
	static ID3D11RenderTargetView* WindowRTV();
	static RenderTargetD3D11* RenderRTV();
	static RenderTargetD3D11* BackBuffer();
	static ID3D11UnorderedAccessView* UAV();
	static ID3D11ShaderResourceView* SRV();
	static D3D11_VIEWPORT Viewport();

private:
	// D3D11 Components
	static ID3D11Device* s_device;
	static ID3D11DeviceContext* s_immediateContext;
	static IDXGISwapChain* s_swapChain;
	static ID3D11RenderTargetView* s_windowRTV;
	static RenderTargetD3D11 s_renderRTV;
	static RenderTargetD3D11 s_backBuffer;
	static ID3D11UnorderedAccessView* s_uav;
	static ID3D11ShaderResourceView* s_srv;
	static D3D11_VIEWPORT s_viewport;
};