#include "stdafx.h"
#include "RenderTargetD3D11.h"

RenderTargetD3D11::RenderTargetD3D11(UINT width, UINT height, DXGI_FORMAT format, bool createSRV, bool createUAV)
{
	Initialize(width, height, format, createSRV, createUAV);
}

RenderTargetD3D11::RenderTargetD3D11(IDXGISwapChain* swapChain)
{
	Initialize(swapChain);
}

RenderTargetD3D11::~RenderTargetD3D11()
{
	Release();
}

void RenderTargetD3D11::Initialize(UINT width, UINT height, DXGI_FORMAT format, bool createSRV, bool createUAV)
{	
	m_width = width;
	m_height = height;
	m_format = format;
	m_hasSRV = createSRV;
	m_hasUAV = createUAV;

	// Createing texture
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width				 = m_width;
	textureDesc.Height				 = m_height;
	textureDesc.MipLevels			 = 1;
	textureDesc.ArraySize			 = 1;
	textureDesc.Format				 = format;
	textureDesc.SampleDesc.Count	 = 1;
	textureDesc.SampleDesc.Quality	 = 0;
	textureDesc.Usage				 = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags			 = D3D11_BIND_RENDER_TARGET;
	if (createSRV)
		textureDesc.BindFlags		 |= D3D11_BIND_SHADER_RESOURCE;
	if (createUAV)
		textureDesc.BindFlags		 |= D3D11_BIND_UNORDERED_ACCESS;
	textureDesc.CPUAccessFlags	     = 0;
	textureDesc.MiscFlags			 = 0;

	HR(DeviceManager::Device()->CreateTexture2D(&textureDesc, nullptr, &m_texture));

	// Creating SRV
	if (createSRV)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels       = 1;

		HR(DeviceManager::Device()->CreateShaderResourceView(m_texture, &srvDesc, &m_srv));
	}

	// Create UAV
	if (createUAV)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = textureDesc.Format;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = 0;

		HR(DeviceManager::Device()->CreateUnorderedAccessView(m_texture, &uavDesc, &m_uav));
	}

	// Creating RTV
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	HR(DeviceManager::Device()->CreateRenderTargetView(m_texture, &rtvDesc, &m_rtv));
}

void RenderTargetD3D11::Initialize(IDXGISwapChain* swapChain)
{
	m_hasSRV = false;
	m_hasUAV = false;

    m_texture = nullptr;
    HR(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&m_texture));

	D3D11_TEXTURE2D_DESC desc;
	m_texture->GetDesc(&desc);
	m_width = desc.Width;
	m_height = desc.Height;
	m_format = desc.Format;

    HR(DeviceManager::Device()->CreateRenderTargetView(m_texture, nullptr, &m_rtv));
}

void RenderTargetD3D11::Resize(UINT width, UINT height)
{
	if (width == 0 || height == 0)
        return;

    if (width == m_width && height == m_height)
        return;

	Release();
	Initialize(width, height, m_format, m_hasSRV, m_hasUAV);
}

void RenderTargetD3D11::Copy(RenderTargetD3D11* other)
{
	// Unbind SRV
	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	DeviceManager::Context()->PSSetShaderResources(0, 1, nullSRV);

	// Unbind UAV
	ID3D11UnorderedAccessView* nullUAV[1] = { nullptr };
	UINT initialCounts[1] = { 0 };
	DeviceManager::Context()->CSSetUnorderedAccessViews(0, 1, nullUAV, initialCounts);

	// Unbind RTV
	ID3D11RenderTargetView* nullRTV[1] = { nullptr };
	DeviceManager::Context()->OMSetRenderTargets(1, nullRTV, nullptr);

	// Copy
	if (m_format == other->m_format && m_width == other->m_width && m_height == other->m_height)
		DeviceManager::Context()->CopyResource(m_texture, other->m_texture);
}

ID3D11RenderTargetView* RenderTargetD3D11::GetRTV() const
{
	return m_rtv;
}

ID3D11UnorderedAccessView* RenderTargetD3D11::GetUAV() const
{
	return m_uav;
}

ID3D11ShaderResourceView* RenderTargetD3D11::GetSRV() const
{
	return m_srv;
}

void RenderTargetD3D11::Release()
{	
	ReleaseCOM(m_texture);
	ReleaseCOM(m_srv);
	ReleaseCOM(m_uav);
	ReleaseCOM(m_rtv);
}
