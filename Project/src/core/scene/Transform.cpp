#include "stdafx.h"

#include "Transform.h"

Transform::Transform():
	m_position(DX::XMFLOAT3(0, 0, 0)), m_rotation(DX::XMFLOAT3(0, 0, 0)), m_scale(DX::XMFLOAT3(1, 1, 1)), m_worldMat(DX::XMMatrixIdentity()), m_isDirty(true)
{
}

Transform::Transform(DX::XMFLOAT3 position, DX::XMFLOAT3 rotation, DX::XMFLOAT3 scale):
	m_position(position), m_rotation(rotation), m_scale(scale), m_isDirty(true)
{
	CreateWorldMatrix();
}

void Transform::SetPosition(DX::XMFLOAT3 position)
{
	m_isDirty = true;
	m_position = position;
}

void Transform::SetRotation(DX::XMFLOAT3 rotation)
{
	m_isDirty = true;
	m_rotation = rotation;
}

void Transform::SetScale(DX::XMFLOAT3 scale)
{
	m_isDirty = true;
	m_scale = scale;
}

void Transform::Move(DX::XMFLOAT3 delta)
{
	m_isDirty = true;
	DX::XMVECTOR newPos = DX::XMVectorAdd(DX::XMLoadFloat3(&delta), DX::XMLoadFloat3(&m_position));
	DX::XMStoreFloat3(&m_position, newPos);
}

void Transform::Move(DX::XMFLOAT3 direction, float amount)
{
	m_isDirty = true;
	DX::XMVECTOR delta = DX::XMVectorScale(DX::XMLoadFloat3(&direction), amount);
	DX::XMVECTOR newPos = DX::XMVectorAdd(delta, DX::XMLoadFloat3(&m_position));
	DX::XMStoreFloat3(&m_position, newPos);
}

void Transform::Rotate(DX::XMFLOAT3 rotation)
{
	m_isDirty = true;
	DX::XMVECTOR newRot = DX::XMVectorAdd(DX::XMLoadFloat3(&rotation), DX::XMLoadFloat3(&m_rotation));
	DX::XMStoreFloat3(&m_rotation, newRot);
}

void Transform::Scale(DX::XMFLOAT3 scale)
{
	m_isDirty = true;
	DX::XMVECTOR newScale = DX::XMVectorAdd(DX::XMLoadFloat3(&scale), DX::XMLoadFloat3(&m_scale));
	DX::XMStoreFloat3(&m_scale, newScale);
}

DX::XMFLOAT3 Transform::GetPosition() const
{
	return m_position;
}

DX::XMFLOAT3 Transform::GetRotation() const
{
	return m_rotation;
}

DX::XMFLOAT3 Transform::GetScale() const
{
	return m_scale;
}

DX::XMMATRIX Transform::GetWorldMatrix()
{
	if (m_isDirty)
		CreateWorldMatrix();
	return m_worldMat;
}

void Transform::RenderUI()
{
	const float min = -100.0f;
	const float max = 100.0f;

	if (ImGui::TreeNode("Transform"))
	{
		if (ImGui::TreeNode("Position"))
		{
			m_isDirty |= ImGui::DragFloat("x-pos", &m_position.x, 0.01f, min, max, "%.2f");
			m_isDirty |= ImGui::DragFloat("y-pos", &m_position.y, 0.01f, min, max, "%.2f");
			m_isDirty |= ImGui::DragFloat("z-pos", &m_position.z, 0.01f, min, max, "%.2f");

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Rotation"))
		{
			m_isDirty |= ImGui::DragFloat("x", &m_rotation.x, 0.01f, min, max, "%.2f");
			m_isDirty |= ImGui::DragFloat("y", &m_rotation.y, 0.01f, min, max, "%.2f");
			m_isDirty |= ImGui::DragFloat("z", &m_rotation.z, 0.01f, min, max, "%.2f");

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Scale"))
		{
			m_isDirty |= ImGui::DragFloat("x", &m_scale.x, 0.01f, min, max, "%.2f");
			m_isDirty |= ImGui::DragFloat("y", &m_scale.y, 0.01f, min, max, "%.2f");
			m_isDirty |= ImGui::DragFloat("z", &m_scale.z, 0.01f, min, max, "%.2f");

			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
}

bool Transform::IsDirty() const
{
	return m_isDirty;
}

void Transform::CreateWorldMatrix()
{
	m_worldMat = DX::XMMatrixIdentity();

	// Scale
	m_worldMat *= DX::XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);

	// Rotation
	m_worldMat *= DX::XMMatrixRotationY(m_rotation.y);
	m_worldMat *= DX::XMMatrixRotationX(m_rotation.x);
	m_worldMat *= DX::XMMatrixRotationZ(m_rotation.z);

	// Translation
	m_worldMat *= DX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);

	m_worldMat = DX::XMMatrixTranspose(m_worldMat);

	m_isDirty = false;
	
	// Use parent coordinate system
	/*
	if (m_parent != nullptr && m_useParentSpace)
	{
		m_parent->UpdateWorldMat(transpose);
		m_worldMat = DX::XMMatrixMultiply(m_parent->m_worldMat, m_worldMat);
	}
	*/
}
