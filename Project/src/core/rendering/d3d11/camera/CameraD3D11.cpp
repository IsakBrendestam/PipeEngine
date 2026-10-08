#include "stdafx.h"

#include "CameraD3D11.h"

CameraD3D11::CameraD3D11(const ProjectionInfo& projectionInfo, const DirectX::XMFLOAT3& initialPosition, const DX::XMFLOAT3& initialRotation)
{
	Initialize(projectionInfo, initialPosition, initialRotation);
}

CameraD3D11::~CameraD3D11()
{
	delete m_cameraBuffer;
}

void CameraD3D11::Initialize(const ProjectionInfo& projectionInfo, const DirectX::XMFLOAT3& initialPosition, const DX::XMFLOAT3& initialRotation)
{
	m_freezeForward = false;
	m_freezeRight = false;
	m_freezeUp = true;

	m_position = initialPosition;
	m_projInfo = projectionInfo;

	auto data = CalculateBufferData();

	//m_rotation = initialRotation;

	m_cameraBuffer = new ConstantBufferD3D11(2*sizeof(DX::XMVECTOR), true, data.data());

	RotateForward(initialRotation.z);
	RotateRight(initialRotation.x);
	RotateUp(initialRotation.y);
}

void CameraD3D11::Update(double deltaTime)
{
	// Internal updates
}

void CameraD3D11::MoveInDirection(float amount, const DX::XMFLOAT3& direction)
{
	m_position.x += direction.x * amount;
	m_position.y += direction.y * amount;
	m_position.z += direction.z * amount;
}

void CameraD3D11::RotateAroundAxis(float amount, const DX::XMFLOAT3& axis)
{
	DX::XMMATRIX rotMat = DX::XMMatrixRotationAxis(DX::XMVector3Normalize(DX::XMLoadFloat3(&axis)), amount);;

	DX::XMStoreFloat3x3(&m_rotationMatrix, rotMat);

	//m_rotationMatrix = DX::XMMatrixMultiply(DX::XMLoadFloat3x3(&m_rotationMatrix), rotMat);

	DX::XMVECTOR newFVal = DX::XMVector3TransformCoord(DX::XMLoadFloat3(&m_forward), rotMat);
	DX::XMVector3Normalize(newFVal);
	DX::XMStoreFloat3(&m_forward, newFVal);

	if (m_forward.y <= -1)
	{
		m_up.y = -1;
	}

	DX::XMVECTOR newUVal = DX::XMVector3TransformCoord(DX::XMLoadFloat3(&m_up), rotMat);
	DX::XMVector3Normalize(newUVal);
	DX::XMStoreFloat3(&m_up, newUVal);

	DX::XMVECTOR newRVal = DX::XMVector3TransformCoord(DX::XMLoadFloat3(&m_right), rotMat);
	DX::XMVector3Normalize(newRVal);
	DX::XMStoreFloat3(&m_right, newRVal);

	m_rotation.x -= atan2(m_rotationMatrix._32, m_rotationMatrix._22);
	m_rotation.z -= asin(-m_rotationMatrix._12);
	m_rotation.y -= atan2(m_rotationMatrix._13, m_rotationMatrix._11);

	/*
	m_rotation.x = acosf(m_forward.z) * 2;
	std::cout << m_forward.x << " : " << acosf(m_forward.z)*2 << std::endl;
	m_rotation.z = acosf(m_forward.x);
	*/
}

std::array<DX::XMVECTOR, 2> CameraD3D11::CalculateBufferData()
{
	DX::XMVECTOR position = DX::XMLoadFloat3(&m_position);
	DX::XMVECTOR focal = DX::XMVectorAdd(position, DX::XMLoadFloat3(&m_forward));

	DX::XMVECTOR cameraDirection = DX::XMVectorSubtract(focal, position);

	return {
		position,
		cameraDirection
	};
}

void CameraD3D11::SetPosition(const DX::XMFLOAT3& position)
{
	m_position = position;
}

void CameraD3D11::MoveForward(float amount)
{
	MoveInDirection(amount, m_forward);
}

void CameraD3D11::MoveRight(float amount)
{
	MoveInDirection(amount, m_right);
}

void CameraD3D11::MoveUp(float amount)
{
	MoveInDirection(amount, m_worldUp);
}

void CameraD3D11::RotateForward(float amount)
{
	RotateAroundAxis(amount, m_forward);
}

void CameraD3D11::RotateRight(float amount)
{
	RotateAroundAxis(amount, m_right);
}

void CameraD3D11::RotateUp(float amount)
{
	RotateAroundAxis(amount, m_worldUp);
}

void CameraD3D11::SetForward(DX::XMFLOAT3 forward)
{
	m_forward = forward;
	DX::XMStoreFloat3(&m_right, DX::XMVector3Cross(DX::XMLoadFloat3(&m_up), DX::XMLoadFloat3(&m_forward)));
}

const DX::XMFLOAT3& CameraD3D11::GetPosition() const
{
	return m_position;
}

const DX::XMFLOAT3& CameraD3D11::GetRotation() const
{
	return m_rotation;
}

const DX::XMFLOAT3& CameraD3D11::GetForward() const
{
	return m_forward;
}

const DX::XMFLOAT3& CameraD3D11::GetRight() const
{
	return m_right;
}

const DX::XMFLOAT3& CameraD3D11::GetUp() const
{
	return m_up;
}

const float CameraD3D11::GetFOV() const
{
	return m_projInfo.fovAngleY;
}

void CameraD3D11::SetFOV(float fov)
{
	m_projInfo.fovAngleY = fov;
}

const ProjectionInfo& CameraD3D11::GetProjection() const
{
	return m_projInfo;
}

void CameraD3D11::SetProjection(const ProjectionInfo& projection)
{
	m_projInfo = projection;
}

void CameraD3D11::UpdateInternalConstantBuffer()
{
	auto data = CalculateBufferData();
	m_cameraBuffer->UpdateBuffer(data.data());
}

ID3D11Buffer* CameraD3D11::GetConstantBuffer() const
{
	return m_cameraBuffer->GetBuffer();
}

DX::XMFLOAT4X4 CameraD3D11::GetViewProjectionMatrix() const
{
	DX::XMMATRIX view = GetViewMatrix();
	DX::XMMATRIX projection = GetProjectionMatrix();

	DX::XMFLOAT4X4 viewProjMat;
	DX::XMStoreFloat4x4(&viewProjMat, DX::XMMatrixTranspose(view * projection));

	return viewProjMat;
}

DX::XMMATRIX CameraD3D11::GetProjectionMatrix() const
{
	return	DX::XMMatrixPerspectiveFovLH(m_projInfo.fovAngleY, m_projInfo.aspectRatio, m_projInfo.nearZ, m_projInfo.farZ);
}

DX::XMMATRIX CameraD3D11::GetInverseProjectionMatrix() const
{
	return DX::XMMatrixInverse(nullptr, GetProjectionMatrix());
}

DX::XMMATRIX CameraD3D11::GetViewMatrix() const
{
	DX::XMVECTOR position = DX::XMLoadFloat3(&m_position);
	DX::XMVECTOR focal = DX::XMVectorAdd(position, DX::XMLoadFloat3(&m_forward));

	DX::XMMATRIX view = DX::XMMatrixLookAtLH(position, focal, DX::XMLoadFloat3(&m_up));
	return view;
}

DX::BoundingFrustum CameraD3D11::GetFrustum() const
{
	DX::BoundingFrustum frustum;
	frustum.CreateFromMatrix(frustum, GetProjectionMatrix());
	frustum.Transform(frustum, GetWorldMatrix());
	return frustum;
}

DX::XMMATRIX CameraD3D11::GetWorldMatrix() const
{
	DX::XMVECTOR det;
	return  DX::XMMatrixInverse(&det, GetViewMatrix());
}
