#pragma once

#include "CameraD3D11.h"

class CameraOrthographicD3D11 : public CameraD3D11
{
public:
	CameraOrthographicD3D11(const ProjectionInfo& projectionInfo, float width, float height,
		const DX::XMFLOAT3& initialPosition = DX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		const DX::XMFLOAT3& initialRotation = DX::XMFLOAT3(0.0f, 0.0f, 0.0f));

	DX::XMFLOAT4X4 GetViewProjectionMatrix() const override;

private:
	float m_width, m_height;
};