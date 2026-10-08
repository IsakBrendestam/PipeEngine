#pragma once

#include <d3d11_4.h>

#include "core/rendering/d3d11/buffers/StructuredBufferD3D11.h"

#include "core/rendering/d3d11/resources/ShaderD3D11.h"
#include "core/rendering/d3d11/resources/SamplerD3D11.h"

#include "platform/D3D11Helper.h"

#include "systems/LightManager.h"

class ShadowMapD3D11
{
public:
	ShadowMapD3D11() = delete;
	ShadowMapD3D11(UINT width, UINT height, UINT nElements, bool dynamic);
	~ShadowMapD3D11();

	void Initialize(UINT width, UINT height, UINT nElements, bool dynamic);
	void Deconstruct();

	ID3D11DepthStencilView* GetDSV(int index);

	void BindShader();
	D3D11_VIEWPORT GetViewport() const;

	ID3D11ShaderResourceView* GetSRV();
	ID3D11SamplerState* GetSampler();

private:
	UINT m_width, m_height;
	UINT m_nElements;

	ID3D11Texture2D* m_texture;
	ID3D11DepthStencilView** m_dsv;
	ID3D11ShaderResourceView* m_srv;

	D3D11_VIEWPORT m_viewport;

	ShaderD3D11* m_vertexShader;
	SamplerD3D11* m_sampler;
};