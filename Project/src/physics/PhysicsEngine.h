#pragma once

#include <vector>
#include "core/scene/Entity.h"
#include "core/scene/Scene.h"
#include "collision/CollisionHandler.h"

class PhysicsEngine
{
public:
	void Initialize(Scene* scene);
	void Update(Scene* scene);

private:
	void Step(double stepSize);

private:
	double m_timeAccumulation = 0;

	Scene* m_scene = nullptr;
	std::vector<Entity*> m_entities;

	CollisionHandler m_collisionHandler;
};
