#include "stdafx.h"
#include "PhysicsEngine.h"

void PhysicsEngine::Initialize(Scene* scene)
{
	m_collisionHandler.Initialize(scene);
}

void PhysicsEngine::Update(Scene* scene)
{
	if (!scene) return;

	m_scene = scene;

	constexpr double stepSize = 0.001f;
	m_timeAccumulation += g_time.DeltaTime<double>();
	while (m_timeAccumulation >= stepSize)
	{
		Step(stepSize);
		m_timeAccumulation -= stepSize;
	}

	// Update Physics
	m_collisionHandler.CollisionStep(m_scene);
}

void PhysicsEngine::Step(double stepSize)
{
	m_scene->InternalPhysicsUpdate(stepSize);
}
