#pragma once

#include <DirectXMath.h>


namespace DX = DirectX;

class Transform
{
public:
	Transform();
	Transform(DX::XMFLOAT3 position, DX::XMFLOAT3 rotation, DX::XMFLOAT3 scale);
	~Transform() = default;

	void SetPosition(DX::XMFLOAT3 position);
	void SetRotation(DX::XMFLOAT3 rotation);
	void SetScale(DX::XMFLOAT3 scale);

	void Move(DX::XMFLOAT3 delta);
	void Move(DX::XMFLOAT3 direction, float amount);
	void Rotate(DX::XMFLOAT3 rotation);
	void Scale(DX::XMFLOAT3 scale);

	DX::XMFLOAT3 GetPosition() const;
	DX::XMFLOAT3 GetRotation() const;
	DX::XMFLOAT3 GetScale() const;

	DX::XMMATRIX GetWorldMatrix();

	void RenderUI();

	bool IsDirty() const;

private:
	void CreateWorldMatrix();

private:
	DX::XMFLOAT3 m_position, m_rotation, m_scale;
	DX::XMMATRIX m_worldMat;

	bool m_isDirty;
};

