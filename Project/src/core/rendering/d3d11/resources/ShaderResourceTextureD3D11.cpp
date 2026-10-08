#include "stdafx.h"

#include "ShaderResourceTextureD3D11.h"

#include "core/rendering/d3d11/DeviceManager.h"

ShaderResourceTextureD3D11::ShaderResourceTextureD3D11(UINT width, UINT height, void* textureData, DXGI_FORMAT format, unsigned char forcedChannels):
	m_forcedChannels(forcedChannels)
{
	Initialize(width, height, textureData, format);
}

ShaderResourceTextureD3D11::ShaderResourceTextureD3D11(const char* pathToTextureFile, DXGI_FORMAT format, unsigned char forcedChannels):
	m_forcedChannels(forcedChannels)
{
	Initialize(pathToTextureFile, format);
}

ShaderResourceTextureD3D11::~ShaderResourceTextureD3D11()
{
	ReleaseCOM(m_texture);
	ReleaseCOM(m_srv);
}

void ShaderResourceTextureD3D11::Initialize(UINT width, UINT height, void* textureData, DXGI_FORMAT format)
{
	m_width = width;
	m_height = height;

	// Defining D3D11 texture
	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = format;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	/*
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = textureData;
	data.SysMemPitch = width * m_forcedChannels;
	data.SysMemSlicePitch = 0;
	*/

	// Creating Texture
	HR(DeviceManager::Device()->CreateTexture2D(&textureDesc, nullptr, &m_texture));
	
	// Loading mip level 0
	DeviceManager::Context()->UpdateSubresource(m_texture, 0, nullptr, textureData, width * m_forcedChannels, 0);

	// Defining Resource View
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1; // Use all mip levels

	// Creaating Resource View
	HR(DeviceManager::Device()->CreateShaderResourceView(m_texture, &srvDesc, &m_srv));

	// Generate mip maps
	DeviceManager::Context()->GenerateMips(m_srv);

	ReleaseCOM(m_texture);
}

void ShaderResourceTextureD3D11::Initialize(const char* pathToTextureFile, DXGI_FORMAT format)
{
	int width = 0, height = 0;

	unsigned char* textureData = stbi_load(pathToTextureFile, &width, &height, nullptr, m_forcedChannels);

	if (textureData == NULL)
	{
		DebugLog("Failed loading texture: " + std::string(pathToTextureFile));
		return;
	}

	Initialize(width, height, textureData, format);

	stbi_image_free(textureData);
}

ID3D11ShaderResourceView* ShaderResourceTextureD3D11::GetSRV() const
{
	return m_srv;
}

UINT ShaderResourceTextureD3D11::GetWidth() const
{
	return m_width;
}

UINT ShaderResourceTextureD3D11::GetHeight() const
{
	return m_height;
}
