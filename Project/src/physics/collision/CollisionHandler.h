#pragma once

#include "Colliders.h"
#include "Intersections.h"

#include <set>

class Scene;

class CollisionHandler
{
public:
	CollisionHandler() = default;
	~CollisionHandler() = default;

	bool Initialize(const Scene *scene);

	bool CheckCollision(const Collisions::Collider *col, const Scene *scene, Collisions::CollisionData &data);

	void CollisionStep(const Scene* scene);

private:
	void Reset();
	void BuildColliderList(const Scene* scene);

	void ResetStepState();
	void ResolveCollisions();
	void CheckCollisions();
	void ResolveCollisionForces();
	void SyncColliders();

	void InvokeCallbacks();


private:
	std::vector<std::pair<Entity*, Collisions::Collider*>> m_entitiesToCheck;

	std::unordered_multimap<Entity*, Collisions::CollisionData> m_intersections,
																m_exiting,
																m_entering;

	bool m_canResolve = false;

	int m_nSteps = 4;
};


