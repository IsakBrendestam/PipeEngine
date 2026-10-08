#include "stdafx.h"

#include "GBufferD3D11.h"

#include "core/rendering/d3d11/DeviceManager.h"
#include "core/rendering/d3d11/Resources/RenderTargetD3D11.h"

GBufferD3D11::GBufferD3D11(UINT width, UINT height)
{
	Initialize(width, height);
}

GBufferD3D11::~GBufferD3D11()
{
	ReleaseCOM(m_texture);
	ReleaseCOM(m_srv);
	ReleaseCOM(m_rtv);
}

void GBufferD3D11::Initialize(UINT width, UINT height)
{
	m_width = width;
	m_height = height;

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	// Creating Texture
	HR(DeviceManager::Device()->CreateTexture2D(&desc, nullptr, &m_texture));

	// Createing SRV
	HR(DeviceManager::Device()->CreateShaderResourceView(m_texture, nullptr, &m_srv));

	// Creating RTV
	HR(DeviceManager::Device()->CreateRenderTargetView(m_texture, nullptr, &m_rtv));

	//backBuffer->Release();
	ReleaseCOM(m_texture);
}

void GBufferD3D11::Clear(const float clearColor[4])
{
	DeviceManager::Context()->ClearRenderTargetView(m_rtv, clearColor);
}

ID3D11ShaderResourceView* GBufferD3D11::GetSRV() const
{
	return m_srv;
}

ID3D11RenderTargetView* GBufferD3D11::GetRTV() const
{
	return m_rtv;
}

UINT GBufferD3D11::GetWidth() const
{
	return m_width;
}

UINT GBufferD3D11::GetHeight() const
{
	return m_height;
}
