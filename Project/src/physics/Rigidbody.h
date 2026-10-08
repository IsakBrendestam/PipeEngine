#pragma once

#include <vector>
#include <DirectXMath.h>
namespace DX = DirectX;

#include "collision/Intersections.h"

class Entity;

class Rigidbody
{
public:
	Rigidbody(Entity* entity, float mass);
	~Rigidbody() = default;

	void AddImpulse(const DX::XMVECTOR& impulse);

	void AddForce(const DX::XMVECTOR& force);
	void AddConstantForce(const DX::XMVECTOR& force);
	void ApplyForces(double deltaTime);

	void ResolveCollisionForces(Collisions::CollisionData data);

	void RenderUI();

private:
	void ApplyConstantForces();
	void ApplyFrictionImpulse(const DX::XMVECTOR& relativeVelocity, const DX::XMVECTOR& normal, float normalImpulse);
	void ApplyFriction(const DX::XMVECTOR& normal, const DX::XMVECTOR& relativeVelocity, float mu);

	void Reset();

private:
	Entity* m_entity;
	float m_mass;

	bool m_colliding;

	std::vector<DX::XMVECTOR> m_constantForces;

	DX::XMVECTOR m_netForce			= DX::XMVectorZero(),
				 m_acceleration		= DX::XMVectorZero(),
				 m_velocity			= DX::XMVectorZero();

	DX::XMVECTOR m_normalForce		= DX::XMVectorZero();
};
