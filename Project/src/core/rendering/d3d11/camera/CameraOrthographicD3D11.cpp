#include "stdafx.h"

#include "CameraOrthographicD3D11.h"

#include "systems/WindowManager.h"

CameraOrthographicD3D11::CameraOrthographicD3D11(const ProjectionInfo& projectionInfo, float width, float height, const DX::XMFLOAT3& initialPosition, const DX::XMFLOAT3& initialRotation)
{
	m_width = width;
	m_height = height;
	CameraD3D11::Initialize(projectionInfo, initialPosition, initialRotation);
}

DX::XMFLOAT4X4 CameraOrthographicD3D11::GetViewProjectionMatrix() const
{
	DX::XMVECTOR position = DX::XMLoadFloat3(&m_position);
	DX::XMVECTOR focal = DX::XMVectorAdd(position, DX::XMLoadFloat3(&m_forward));

	DX::XMMATRIX view = DX::XMMatrixLookAtLH(position, focal, DX::XMLoadFloat3(&m_up));
	DX::XMMATRIX projection = DX::XMMatrixOrthographicLH(m_width, m_height, m_projInfo.nearZ, m_projInfo.farZ);

	DX::XMFLOAT4X4 viewProjMat;
	DX::XMStoreFloat4x4(&viewProjMat, DX::XMMatrixTranspose(view * projection));

	return viewProjMat;
}
