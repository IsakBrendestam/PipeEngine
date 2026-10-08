#pragma once

#include <d3d11.h>

#include "core/graphics/Material.h"
#include "physics/collision/Colliders.h"
#include "assets/ResourceManager.h"

#include "core/UniqueID.h"
#include "Transform.h"
#include "physics/Rigidbody.h"

class Entity
{
public:
	Entity();
	~Entity();

	const bool operator<(const Entity& other) const;
	const bool operator==(const Entity& other) const;

	ID GetID() const;

	void SetMesh(ID mesh);
	ID GetMesh() const;

	void SetMaterial(const MaterialMaps& material);
	const MaterialMaps& GetMaterial() const;

	void SetSampler(ID sampler);
	ID GetSampler() const;

	void SetTopology(D3D11_PRIMITIVE_TOPOLOGY topoloy);
	D3D11_PRIMITIVE_TOPOLOGY GetTopology() const;

	void SetTransform(const Transform& transform); // TODO: Remove
	Transform& GetTransform();

	void SetCollider(Collisions::Collider* collider);
	Collisions::Collider* GetCollider() const;

	void SetRigidbody(Rigidbody* rigidbody);
	Rigidbody* GetRigidbody() const;

	void RenderUI(bool &changed, ResourceManager& resourceManager);

	void Update();
	void PhysicsUpdate(const double& stepSize);
	void TransformCollider();

private:
	ID m_ID = INVALID_ID;

	ID m_mesh = INVALID_ID,
	   m_sampler = INVALID_ID;

	MaterialMaps m_material = g_nullMaterialMap;

	Transform m_transform;
	Rigidbody* m_rigidbody = nullptr;

	Collisions::Collider* m_baseCollider = nullptr;
	Collisions::Collider* m_transformedCollider = nullptr;
	
	DX::XMMATRIX m_rotationMatrix;
	DX::XMVECTOR m_angularVelocity;

	D3D11_PRIMITIVE_TOPOLOGY m_topology;
};