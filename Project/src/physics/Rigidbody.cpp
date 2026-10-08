#include "stdafx.h"

#include "Rigidbody.h"
#include "core/Engine.h"

using namespace Collisions;

Rigidbody::Rigidbody(Entity* entity, float mass):
	m_entity(entity), m_mass(mass), m_colliding(false)
{
	// Gravity
	m_constantForces.push_back(DX::XMVectorSet(0, -9.82f * m_mass, 0, 0));
}

void Rigidbody::AddImpulse(const DX::XMVECTOR& impulse)
{
	m_velocity = DX::XMVectorAdd(m_velocity, DX::XMVectorScale(impulse, 1.0f/m_mass));
}

void Rigidbody::AddForce(const DX::XMVECTOR& force)
{
	m_netForce = DX::XMVectorAdd(m_netForce, force);
}

void Rigidbody::AddConstantForce(const DX::XMVECTOR& force)
{
	m_constantForces.push_back(force);
}

void Rigidbody::ApplyForces(double deltaTime)
{
	DX::XMVECTOR move = DX::XMVectorZero();
	
	ApplyConstantForces();

	m_acceleration = DX::XMVectorScale(m_netForce, 1.0f/m_mass);

	m_velocity = DX::XMVectorMultiplyAdd(m_acceleration, DX::XMVectorReplicate((float)deltaTime), m_velocity);
	move = DX::XMVectorScale(m_velocity, (float)deltaTime);
		
	DX::XMFLOAT3 delta;
	DX::XMStoreFloat3(&delta, move);

	m_entity->GetTransform().Move(delta);

	Reset();
}

void Rigidbody::ResolveCollisionForces(Collisions::CollisionData data)
{
	m_colliding = true;

	DX::XMVECTOR n = DX::XMVectorScale(DX::XMLoadFloat3(&data.normal), -1);
	n = DX::XMVector3Normalize(n);

	float vn = DX::XMVectorGetX(DX::XMVector3Dot(m_velocity, n));
	if (vn <= 0)
		return; // No impulse

	// Normal Impulse

	float restitution = 0.0f;
	float jn = -(1.0f + restitution) * vn;
	jn /= (1.0f / m_mass);

	DX::XMVECTOR impulseN = DX::XMVectorScale(n, jn);
	m_velocity = DX::XMVectorAdd(m_velocity, DX::XMVectorScale(impulseN, 1.0f / m_mass));

	ApplyFrictionImpulse(m_velocity, n, jn);

	/*
	m_normalForce = DX::XMVectorScale(n, vn);
	m_velocity = DX::XMVectorSubtract(m_velocity, m_normalForce);

	ApplyFriction(n, m_velocity, 0.3f);
	*/
}

void Rigidbody::RenderUI()
{
	ImGui::Text("Rigidbody");
	ImGui::DragFloat("Mass", &m_mass, 0.01f, 0, 10000);
}

void Rigidbody::ApplyConstantForces()
{
	for (auto& force : m_constantForces)
		m_netForce = DX::XMVectorAdd(m_netForce, force);
}

void Rigidbody::ApplyFrictionImpulse(const DX::XMVECTOR& relativeVelocity, const DX::XMVECTOR& normal, float normalImpulse)
{
	// Tangential velocity
	float nv = DX::XMVectorGetX(DX::XMVector3Dot(relativeVelocity, normal));
	DX::XMVECTOR vt = DX::XMVectorSubtract(relativeVelocity, DX::XMVectorScale(normal, nv));

	float vtLenSq = DX::XMVectorGetX(DX::XMVector3LengthSq(vt));
	if (vtLenSq < GameMath::EPSILON)
		return;

	DX::XMVECTOR t = DX::XMVector3Normalize(vt);

	// Frictional impulse magnitude
	float mu = 0.3f; // TODO: make dynamic (material based)
	float jt = -DX::XMVectorGetX(DX::XMVector3Dot(relativeVelocity, t));
	jt /= (1.0f / m_mass);

	// Columb clamp
	float maxFriction = std::fabsf(mu * normalImpulse);
	jt = std::clamp(jt, -maxFriction, maxFriction);

	DX::XMVECTOR impulseT = DX::XMVectorScale(t, jt);
	m_velocity = DX::XMVectorAdd(m_velocity, DX::XMVectorScale(impulseT, 1.0f / m_mass));
}

void Rigidbody::ApplyFriction(const DX::XMVECTOR& normal, const DX::XMVECTOR& relativeVelocity, float mu)
{
	float vn = DX::XMVectorGetX(DX::XMVector3Dot(relativeVelocity, normal));

    DX::XMVECTOR vt = DX::XMVectorSubtract(relativeVelocity, DX::XMVectorScale(normal, vn));

    if (DX::XMVectorGetX(DX::XMVector3LengthSq(vt)) < GameMath::EPSILON)
        return;

    DX::XMVECTOR dir = DX::XMVector3Normalize(DX::XMVectorNegate(vt));

    float N = m_mass * 9.82f; // TODO: Find more accurate

    DX::XMVECTOR friction = DX::XMVectorScale(dir, mu * N);

    m_netForce = DX::XMVectorAdd(m_netForce, friction);
}

void Rigidbody::Reset()
{
	m_netForce = DX::XMVectorZero();
	m_normalForce = DX::XMVectorZero();
	m_colliding = false;
}
