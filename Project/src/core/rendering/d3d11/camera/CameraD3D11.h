#pragma once

#include <d3d11_4.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>

#include "platform/D3D11Helper.h"

#include "core/rendering/d3d11/buffers/ConstantBufferD3D11.h"

namespace DX = DirectX;

struct ProjectionInfo
{
	float fovAngleY = 0.0f;
	float aspectRatio = 0.0f;
	float nearZ = 0.0f;
	float farZ = 0.0f;
};

class CameraD3D11
{
public:
	CameraD3D11() = default;
	CameraD3D11(const ProjectionInfo& projectionInfo,
		const DX::XMFLOAT3& initialPosition = DX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		const DX::XMFLOAT3& initialRotation = DX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	~CameraD3D11();
	CameraD3D11(const CameraD3D11& other) = delete;
	CameraD3D11& operator=(const CameraD3D11& other) = delete;
	CameraD3D11(CameraD3D11&& other) = default;
	CameraD3D11& operator=(CameraD3D11&& other) = default;

	void Initialize(const ProjectionInfo& projectionInfo,
		const DX::XMFLOAT3& initialPosition = DX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		const DX::XMFLOAT3& initialRotation = DX::XMFLOAT3(0.0f, 0.0f, 0.0f));

	void Update(double deltaTime);

	void SetPosition(const DX::XMFLOAT3& position);
	void MoveForward(float amount);
	void MoveRight(float amount);
	void MoveUp(float amount);

	void RotateForward(float amount);
	void RotateRight(float amount);
	void RotateUp(float amount);

	void SetForward(DX::XMFLOAT3 forward);

	const DX::XMFLOAT3& GetPosition() const;
	const DX::XMFLOAT3& GetRotation() const;

	const DX::XMFLOAT3& GetForward() const;
	const DX::XMFLOAT3& GetRight() const;
	const DX::XMFLOAT3& GetUp() const;

	const float GetFOV() const;
	void SetFOV(float fov);

	const ProjectionInfo& GetProjection() const;
	void SetProjection(const ProjectionInfo& projection);

	void UpdateInternalConstantBuffer();
	ID3D11Buffer* GetConstantBuffer() const;

	virtual DX::XMFLOAT4X4 GetViewProjectionMatrix() const;
	virtual DX::XMMATRIX GetProjectionMatrix() const;
	virtual DX::XMMATRIX GetInverseProjectionMatrix() const;
	virtual DX::XMMATRIX GetViewMatrix() const;
	virtual DX::BoundingFrustum GetFrustum() const;

	DX::XMMATRIX GetWorldMatrix() const;

protected:
	DX::XMFLOAT3 m_position = { 0.0f, 0.0f, 0.0f };
	DX::XMFLOAT3 m_rotation = { 0.0f, 0.0f, 0.0f };

	DX::XMFLOAT3 m_forward = { 0.0f, 0.0f, 1.0f };
	DX::XMFLOAT3 m_right = { 1.0f, 0.0f, 0.0f };
	DX::XMFLOAT3 m_up = { 0.0f, 1.0f, 0.0f };

	const DX::XMFLOAT3 m_worldForward = { 0.0f, 0.0f, 1.0f };
	const DX::XMFLOAT3 m_worldRight = { 1.0f, 0.0f, 0.0f };
	const DX::XMFLOAT3 m_worldUp = { 0.0f, 1.0f, 0.0f };

	bool m_freezeForward = false, 
		 m_freezeRight = false, 
		 m_freezeUp = false;

	ProjectionInfo m_projInfo;

	ConstantBufferD3D11* m_cameraBuffer = nullptr;

	DX::XMFLOAT3X3 m_rotationMatrix = {};

	void MoveInDirection(float amount, const DX::XMFLOAT3& direction);
	void RotateAroundAxis(float amount, const DX::XMFLOAT3& axis);

	std::array<DX::XMVECTOR, 2> CalculateBufferData();
};