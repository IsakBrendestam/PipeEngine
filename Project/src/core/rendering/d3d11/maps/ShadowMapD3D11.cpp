#include "stdafx.h"

#include "ShadowMapD3D11.h"

#include "core/rendering/d3d11/DeviceManager.h"

ShadowMapD3D11::ShadowMapD3D11(UINT width, UINT height, UINT nElements, bool dynamic)
{
	Initialize(width, height, nElements, dynamic);
}

ShadowMapD3D11::~ShadowMapD3D11()
{
	Deconstruct();
}

void ShadowMapD3D11::Initialize(UINT width, UINT height, UINT nElements, bool dynamic)
{
	m_width = width;
	m_height = height;
	m_nElements = nElements;

	// Creating Texture
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = m_width;
	desc.Height = m_height;
	desc.MipLevels = 1;
	desc.ArraySize = m_nElements;
	desc.Format = DXGI_FORMAT_R32_TYPELESS;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	HR(DeviceManager::Device()->CreateTexture2D(&desc, nullptr, &m_texture));

	// Creating Depth Stencil View
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	dsvDesc.Flags = 0;
	dsvDesc.Texture2DArray.MipSlice = 0;
	dsvDesc.Texture2DArray.ArraySize = 1;

	m_dsv = new ID3D11DepthStencilView*[nElements];

	for (int i = 0; i < m_nElements; i++)
	{
		dsvDesc.Texture2DArray.FirstArraySlice = i;
		HR(DeviceManager::Device()->CreateDepthStencilView(m_texture, &dsvDesc, &m_dsv[i]));
	}

	// Creating Shader Resource View
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.MostDetailedMip = 0;
	srvDesc.Texture2DArray.MipLevels = 1;
	srvDesc.Texture2DArray.FirstArraySlice = 0;
	srvDesc.Texture2DArray.ArraySize = m_nElements;

	HR(DeviceManager::Device()->CreateShaderResourceView(m_texture, &srvDesc, &m_srv));

	// Initialize VertexShader
	const char* shaderName = "ShadowVertexShader";
	const std::string shaderPath = std::format("{}\\{}.{}", SHADER_PATH, shaderName, SHADER_EXTENSION);
	const std::string csoPath = std::format("{}\\{}.cso", CSO_PATH, shaderName);
	m_vertexShader = new ShaderD3D11(ShaderType::VERTEX_SHADER, csoPath.c_str(), shaderPath.c_str());

	// Initialize ViewPort
	m_viewport.TopLeftX = 0;
	m_viewport.TopLeftY = 0;
	m_viewport.Width = static_cast<float>(width);;
	m_viewport.Height = static_cast<float>(height);
	m_viewport.MinDepth = 0;
	m_viewport.MaxDepth = 1;

	m_sampler = new SamplerD3D11(D3D11_TEXTURE_ADDRESS_BORDER, {1, 1, 1, 1});
}

void ShadowMapD3D11::Deconstruct()
{
	ReleaseCOM(m_texture);

	for (int i = 0; i < m_nElements; i++)
		ReleaseCOM(m_dsv[i]);
	delete[] m_dsv;

	ReleaseCOM(m_srv);

	delete m_vertexShader;
	delete m_sampler;
}

ID3D11DepthStencilView* ShadowMapD3D11::GetDSV(int index)
{
	if (index < m_nElements)
		return m_dsv[index];
	return nullptr;
}

void ShadowMapD3D11::BindShader()
{
	m_vertexShader->BindShader();
}

D3D11_VIEWPORT ShadowMapD3D11::GetViewport() const
{
	return m_viewport;
}

ID3D11ShaderResourceView* ShadowMapD3D11::GetSRV()
{
	return m_srv;
}

ID3D11SamplerState* ShadowMapD3D11::GetSampler()
{
	return m_sampler->GetSamplerState();
}
