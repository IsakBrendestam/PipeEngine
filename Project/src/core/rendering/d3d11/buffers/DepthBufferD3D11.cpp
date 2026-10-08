#include "stdafx.h"

#include "DepthBufferD3D11.h"

#include "core/rendering/d3d11/DeviceManager.h"

DepthBufferD3D11::DepthBufferD3D11(UINT width, UINT height, bool hasSRV)
{
	Initialize(width, height, hasSRV);
}

DepthBufferD3D11::~DepthBufferD3D11()
{
    ReleaseCOM(m_texture);
    ReleaseCOM(m_srv);
     
    for (auto& view : m_depthStencilViews)
        ReleaseCOM(view);
}

void DepthBufferD3D11::Initialize(UINT width, UINT height, bool hasSRV, UINT arraySize)
{
	D3D11_TEXTURE2D_DESC textureDesc;
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = arraySize;
    textureDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    HR(DeviceManager::Device()->CreateTexture2D(&textureDesc, nullptr, &m_texture));

    // DSV
    m_depthStencilViews.push_back(nullptr);

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

    HR(DeviceManager::Device()->CreateDepthStencilView(m_texture, &dsvDesc, &m_depthStencilViews[0]));

    // SRV
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

    HR(DeviceManager::Device()->CreateShaderResourceView(m_texture, &srvDesc, &m_srv));
}

ID3D11DepthStencilView* DepthBufferD3D11::GetDSV(UINT arrayIndex) const
{
	return m_depthStencilViews[arrayIndex];
}

ID3D11ShaderResourceView* DepthBufferD3D11::GetSRV() const
{
	return m_srv;
}
