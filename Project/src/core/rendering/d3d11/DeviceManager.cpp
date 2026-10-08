#include "stdafx.h"

#include "DeviceManager.h"

ID3D11Device* DeviceManager::s_device;
ID3D11DeviceContext* DeviceManager::s_immediateContext;
IDXGISwapChain* DeviceManager::s_swapChain;
ID3D11RenderTargetView* DeviceManager::s_windowRTV;
RenderTargetD3D11 DeviceManager::s_renderRTV;
RenderTargetD3D11 DeviceManager::s_backBuffer;
ID3D11UnorderedAccessView* DeviceManager::s_uav;
ID3D11ShaderResourceView* DeviceManager::s_srv;
D3D11_VIEWPORT DeviceManager::s_viewport;

int DeviceManager::Initialize(HWND window, UINT clientWidth, UINT clientHeight)
{
    // Setup D3D11
    if (!SetupD3D11(clientWidth, clientHeight, window, s_device, s_immediateContext, s_swapChain, s_windowRTV, s_uav, s_srv, s_viewport))
    {
        std::cerr << "Failed to setup d3d11!" << std::endl;
        DebugLog("Failed to setup d3d11!");
        return -1;
    }

    s_renderRTV.Initialize(g_engineSettings.renderResolution[0], g_engineSettings.renderResolution[1], DXGI_FORMAT_R16G16B16A16_FLOAT, true, true);

    //if (g_engineSettings.imguiOn)
        s_backBuffer.Initialize(g_engineSettings.renderResolution[0], g_engineSettings.renderResolution[1], DXGI_FORMAT_R8G8B8A8_UNORM, true);
    //else
        //s_backBuffer.Initialize(s_swapChain);

    return 0;
}

void DeviceManager::Deconstruct()
{
    ReleaseCOM(s_immediateContext);
    ReleaseCOM(s_swapChain);
    ReleaseCOM(s_windowRTV);
    ReleaseCOM(s_uav);
    ReleaseCOM(s_srv);

	if (DEBUG)
	{
		ID3D11Debug* debugDevice = nullptr;
		DeviceManager::Device()->QueryInterface(__uuidof(ID3D11Debug), (void**)&debugDevice);

		debugDevice->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);

		debugDevice->Release();
	}

    ReleaseCOM(s_device);
}

void DeviceManager::Resize()
{
}

ID3D11Device* DeviceManager::Device()
{
    return s_device;
}

ID3D11DeviceContext* DeviceManager::Context()
{
    return s_immediateContext;
}

IDXGISwapChain* DeviceManager::SwapChain()
{
    return s_swapChain;
}

ID3D11RenderTargetView* DeviceManager::WindowRTV()
{
    return s_windowRTV;
}

RenderTargetD3D11* DeviceManager::RenderRTV()
{
    return &s_renderRTV;
}

RenderTargetD3D11* DeviceManager::BackBuffer()
{
    return &s_backBuffer;
}

ID3D11UnorderedAccessView* DeviceManager::UAV()
{
    return s_uav;
}

ID3D11ShaderResourceView* DeviceManager::SRV()
{
    return s_srv;
}

D3D11_VIEWPORT DeviceManager::Viewport()
{
    return s_viewport;
}
