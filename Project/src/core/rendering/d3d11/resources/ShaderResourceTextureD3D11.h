#pragma once

#include "platform/D3D11Helper.h"
#include <d3d11_4.h>

class ShaderResourceTextureD3D11
{
private:
	ID3D11Texture2D* m_texture = nullptr;
	ID3D11ShaderResourceView* m_srv = nullptr;
	UINT m_width, m_height;

	unsigned char m_forcedChannels;

public:
	ShaderResourceTextureD3D11() = default;
	ShaderResourceTextureD3D11(UINT width, UINT height, void* textureData, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, unsigned char forcedChannels = 4);
	ShaderResourceTextureD3D11(const char* pathToTextureFile, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM, unsigned char forcedChannels = 4);
	~ShaderResourceTextureD3D11();
	ShaderResourceTextureD3D11(const ShaderResourceTextureD3D11& other) = delete;
	ShaderResourceTextureD3D11& operator=(const ShaderResourceTextureD3D11& other) = delete;
	ShaderResourceTextureD3D11(ShaderResourceTextureD3D11&& other) = delete;
	ShaderResourceTextureD3D11& operator=(ShaderResourceTextureD3D11&& other) = delete;

	void Initialize(UINT width, UINT height, void* textureData, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);
	void Initialize(const char* pathToTextureFile, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);

	ID3D11ShaderResourceView* GetSRV() const;
	UINT GetWidth() const;
	UINT GetHeight() const;
};