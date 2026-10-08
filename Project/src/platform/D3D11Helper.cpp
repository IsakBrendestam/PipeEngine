#include "stdafx.h"

#include "D3D11Helper.h"

bool CreateInterface(UINT width, UINT height, HWND window, ID3D11Device*& device, ID3D11DeviceContext*& immediateContext, IDXGISwapChain*& swapChain)
{
    // swap chain description
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};

    swapChainDesc.BufferDesc.Width = width;
    swapChainDesc.BufferDesc.Height = height;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    // Default
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

    swapChainDesc.BufferUsage = DXGI_USAGE_UNORDERED_ACCESS | DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
    swapChainDesc.BufferCount = 1;
    swapChainDesc.OutputWindow = window;
    swapChainDesc.Windowed = true;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.Flags = 0;

    // Device manager flags
    UINT flags = 0;
        flags = D3D11_CREATE_DEVICE_DEBUG;
    if (DEBUG)
        flags = D3D11_CREATE_DEVICE_DEBUG;

    D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };

    // Create Interfaces
    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, 
        D3D_DRIVER_TYPE_HARDWARE, 
        nullptr, 
        flags, 
        featureLevels, 
        1, 
        D3D11_SDK_VERSION, 
        &swapChainDesc, 
        &swapChain, 
        &device, 
        nullptr, 
        &immediateContext
    );

    return !FAILED(hr);
}

bool CreateRenderTargetView(ID3D11Device* device, IDXGISwapChain* swapChain, ID3D11RenderTargetView*& rtv)
{
    ID3D11Texture2D* backBuffer = nullptr;
    if (FAILED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer))))
    {
        std::cerr << "Failed to get back buffer!" << std::endl;
        return false;
    }

    HRESULT hr = device->CreateRenderTargetView(backBuffer, nullptr, &rtv);
    backBuffer->Release();
    return !FAILED(hr);
}

void CreateUnorderedAccessView(ID3D11Device* device, IDXGISwapChain* swapChain, ID3D11UnorderedAccessView*& uav)
{
    ID3D11Texture2D* backBuffer = nullptr;
    HR(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));

    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
    uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
    uavDesc.Texture2D.MipSlice = 0;

    HR(device->CreateUnorderedAccessView(backBuffer, &uavDesc, &uav));
    backBuffer->Release();
}

void CreateShaderResourceView(ID3D11Device* device, IDXGISwapChain* swapChain, ID3D11ShaderResourceView*& srv)
{
    ID3D11Texture2D* backBuffer = nullptr;
    HR(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;

    HR(device->CreateShaderResourceView(backBuffer, &srvDesc, &srv));
    backBuffer->Release();
}

void SetViewport(D3D11_VIEWPORT& viewport, UINT width, UINT height)
{
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = static_cast<float>(width);
    viewport.Height = static_cast<float>(height);
    viewport.MinDepth = 0;
    viewport.MaxDepth = 1;
}

bool SetupD3D11(UINT width, UINT height, HWND widnow, ID3D11Device*& device, ID3D11DeviceContext*& immediateContext, IDXGISwapChain*& swapChain, ID3D11RenderTargetView*& rtv, ID3D11UnorderedAccessView*& uav, ID3D11ShaderResourceView*& srv, D3D11_VIEWPORT& viewport)
{
    if (!CreateInterface(width, height, widnow, device, immediateContext, swapChain))
    {
        std::cerr << "Failed creating interfaces!" << std::endl;
        return false;
    }

    if (!CreateRenderTargetView(device, swapChain, rtv))
    {
        std::cerr << "Failed creating rtv!" << std::endl;
        return false;
    }

    CreateUnorderedAccessView(device, swapChain, uav);
    CreateShaderResourceView(device, swapChain, srv);

    SetViewport(viewport, g_engineSettings.renderResolution[0], g_engineSettings.renderResolution[1]);

    return true;
}

bool InitializeBackBuffer(IDXGISwapChain* swapChain)
{
    return false;
}
