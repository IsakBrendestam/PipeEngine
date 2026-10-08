#include "stdafx.h"

#include "EnviromentMapD3D11.h"

#include "core/rendering/d3d11/DeviceManager.h"

EnviromentMapD3D11::EnviromentMapD3D11(DX::XMFLOAT3 position)
{
	Initialize(position);
}

EnviromentMapD3D11::~EnviromentMapD3D11()
{
	ReleaseCOM(m_texture);
	ReleaseCOM(m_srv);
	ReleaseCOM(m_uav);

	for (int i = 0; i < 6; i++)
	{
		ReleaseCOM(m_rtvs[i]);
		delete m_cameras[i];
		delete m_depthBuffers[i];
	}

	for (int i = 0; i < m_nGBuffers; i++)
		delete m_gBuffers[i];
}

void EnviromentMapD3D11::Initialize(DX::XMFLOAT3 position)
{
	// Create Texture
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = m_width;
	desc.Height = m_height;
	desc.MipLevels = 1;
	desc.ArraySize = 6;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	HR(DeviceManager::Device()->CreateTexture2D(&desc, nullptr, &m_texture));

	// Create RTVs
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = desc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	rtvDesc.Texture2DArray.ArraySize = 1;
	rtvDesc.Texture2DArray.MipSlice = 0;

	for (int i = 0; i < 6; i++)
	{
		rtvDesc.Texture2DArray.FirstArraySlice = i;
		HR(DeviceManager::Device()->CreateRenderTargetView(m_texture, &rtvDesc, &m_rtvs[i]));
	}

	// Create SRV
	HR(DeviceManager::Device()->CreateShaderResourceView(m_texture, nullptr, &m_srv));

	// Create UAV
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;

	HR(DeviceManager::Device()->CreateUnorderedAccessView(m_texture, &uavDesc, &m_uav));

	ReleaseCOM(m_texture);

	// Create GBuffers
	for (int i = 0; i < 4; i++)
		m_gBuffers[i] = new GBufferD3D11(m_width, m_height);

	// Create Cameras
	ProjectionInfo projInfo;
	projInfo.fovAngleY = DX::XM_PIDIV2;
	projInfo.aspectRatio = m_width/m_height;
	projInfo.nearZ = 0.1f;
	projInfo.farZ = 200;

	// +x -x +y -y +z -z
	float xRotations[6] = { DX::XM_PIDIV2, -DX::XM_PIDIV2, 0, 0, 0, DX::XM_PI };
	float yRotations[6] = { 0, 0, -DX::XM_PIDIV2, DX::XM_PIDIV2, 0, 0 };

	for (int i = 0; i < 6; i++)
	{
		m_cameras[i] = new CameraD3D11(projInfo, position, { yRotations[i], xRotations[i], 0 });
		m_depthBuffers[i] = new DepthBufferD3D11(m_width, m_height);
	}

	m_viewport.TopLeftX = 0;
	m_viewport.TopLeftY = 0;
	m_viewport.Width = static_cast<float>(m_width);
	m_viewport.Height = static_cast<float>(m_height);
	m_viewport.MinDepth = 0;
	m_viewport.MaxDepth = 1;
}

void EnviromentMapD3D11::SetPosition(DX::XMFLOAT3 position)
{
	for (int i = 0; i < 6; i++)
		m_cameras[i]->SetPosition(position);
}

void EnviromentMapD3D11::BeginRender(unsigned int index)
{
	// Clear
	DeviceManager::Context()->ClearRenderTargetView(m_rtvs[index], g_engineSettings.clearColor);
	DeviceManager::Context()->ClearDepthStencilView(m_depthBuffers[0]->GetDSV(0), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);

	// Set Viewport
	DeviceManager::Context()->RSSetViewports(1, &m_viewport);

	if (g_engineSettings.deferredRenderingOn)
	{
		// Clear
		for (int i = 0; i < m_nGBuffers; i++)
			m_gBuffers[i]->Clear(g_engineSettings.clearColor);

		// Render Target
		ID3D11RenderTargetView* omRTVs[m_nGBuffers];
		for (int i = 0; i < m_nGBuffers; i++)
			omRTVs[i] = m_gBuffers[i]->GetRTV();
		DeviceManager::Context()->OMSetRenderTargets(m_nGBuffers, omRTVs, m_depthBuffers[0]->GetDSV(0));
	}
	else
	{
		// Render Target
		DeviceManager::Context()->OMSetRenderTargets(1, &m_rtvs[index], m_depthBuffers[0]->GetDSV(0));
	}
}

void EnviromentMapD3D11::EndRender()
{
	ID3D11ShaderResourceView* psNullSRV = nullptr;
	DeviceManager::Context()->PSSetShaderResources(0, 1, &psNullSRV);

	ID3D11RenderTargetView* nullRTV = nullptr;
	DeviceManager::Context()->OMSetRenderTargets(1, &nullRTV, nullptr);
}

void EnviromentMapD3D11::SetDefferedSrvs()
{
	// Set UAV
	DeviceManager::Context()->CSSetUnorderedAccessViews(0, 1, &m_uav, nullptr);

	// Set SRVs
	ID3D11ShaderResourceView* csSRVs[m_nGBuffers];
	for (int i = 0; i < m_nGBuffers; i++)
		csSRVs[i] = m_gBuffers[i]->GetSRV();
	DeviceManager::Context()->CSSetShaderResources(0, m_nGBuffers, csSRVs);
}

ID3D11ShaderResourceView* EnviromentMapD3D11::GetSRV()
{
	return m_srv;
}

CameraD3D11* EnviromentMapD3D11::GetCamera(unsigned int index) const
{
	return m_cameras[index];
}


