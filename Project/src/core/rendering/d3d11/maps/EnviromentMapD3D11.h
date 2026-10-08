#pragma once

#include <d3d11_4.h>
#include <DirectXMath.h>

#include "systems/WindowManager.h"

#include "platform/D3D11Helper.h"

#include "core/rendering/d3d11/camera/CameraD3D11.h"

#include "core/rendering/d3d11/buffers/DepthBufferD3D11.h"
#include "core/rendering/d3d11/buffers/GBufferD3D11.h"
#include "core/rendering/d3d11/buffers/IndexBufferD3D11.h"

#include "core/EngineSettings.h"

namespace DX = DirectX;

enum TEXTURE_CUBE_FACE_INDEX
{
	POSITIVE_X = 0,
	NEGATIVE_X = 1,
	POSITIVE_Y = 2,
	NEGATIVE_Y = 3,
	POSITIVE_Z = 4,
	NEGATICE_Z = 5
};

class EnviromentMapD3D11
{
public:
	EnviromentMapD3D11(DX::XMFLOAT3 position);
	~EnviromentMapD3D11();

	void Initialize(DX::XMFLOAT3 position);

	void SetPosition(DX::XMFLOAT3 position);

	void BeginRender(unsigned int index);
	void EndRender();

	void SetDefferedSrvs();

	ID3D11ShaderResourceView* GetSRV();
	CameraD3D11* GetCamera(unsigned int index) const;

	unsigned int GetWidth() const { return m_width; }
	unsigned int GetHeight() const { return m_height; }

private:
	ID3D11Texture2D* m_texture;
	ID3D11RenderTargetView* m_rtvs[6];
	ID3D11ShaderResourceView* m_srv;
	ID3D11UnorderedAccessView* m_uav;

	static const unsigned int m_nGBuffers = 4;
	GBufferD3D11* m_gBuffers[m_nGBuffers];

	CameraD3D11* m_cameras[6];
	DepthBufferD3D11* m_depthBuffers[6];
	D3D11_VIEWPORT m_viewport;

	const unsigned int m_width = 2048;
	const unsigned int m_height = 2048;
};