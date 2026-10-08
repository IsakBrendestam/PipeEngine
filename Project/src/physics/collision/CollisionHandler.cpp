#include "stdafx.h"
#include "CollisionHandler.h"
#include "core/scene/Scene.h"

#include <vector>
#include <map>
#include <algorithm>

using namespace Collisions;
using namespace DirectX;

constexpr float kSlop = 0.005f;
constexpr float kBias = 0.6f;

bool CollisionHandler::Initialize(const Scene *scene)
{
	std::vector<Entity *> entities;
	scene->GetEntities(entities);

	m_entitiesToCheck.clear();

	m_entitiesToCheck.reserve(entities.size());

	// Check if entities have colliders
	for (int i = 0; i < entities.size(); i++)
	{
		Entity *ent = entities[i];
		Collider *collider = ent->GetCollider();

		if (collider)
		{
			if (collider->colliderType != NULL_COLLIDER)
			{
				// Reset Intersecting status
				// Add collider and entity to be checked
				m_entitiesToCheck.push_back(std::make_pair(ent, ent->GetCollider()));
			}
		}
	}

	return true;
}

bool CollisionHandler::CheckCollision(const Collider *col, const Scene *scene, CollisionData &data)
{
	//if (sh->GetRecalculateColliders())
	{
		std::vector<Entity *> entities;
		scene->GetEntities(entities);

		m_entitiesToCheck.clear();

		// Check if entities have colliders
		for (int i = 0; i < entities.size(); i++)
		{
			Entity *ent = entities[i];

			Collider *col = ent->GetCollider();
			if (col)
			{
				if (col->colliderType != NULL_COLLIDER)
				{
					// Add collider and entity to be checked
					m_entitiesToCheck.push_back(std::make_pair(ent, ent->GetCollider()));
				}
			}
		}
	}

	for (int j = 0; j < m_entitiesToCheck.size(); j++)
	{
		const Collider *col2 = m_entitiesToCheck[j].second;

		if (col2->GetMin().x > col->GetMax().x) break;
		if (col2->GetMin().y > col->GetMax().y || col->GetMin().y > col2->GetMax().y) continue;
		if (col2->GetMin().z > col->GetMax().z || col->GetMin().z > col2->GetMax().z) continue;

		if (CheckIntersection(col, col2, data))
		{
			data.other = col2;
			return true;
		}
	}


	return false;
}

void CollisionHandler::CollisionStep(const Scene* scene)
{
    ZoneScopedC(RandomUniqueColor());

	Reset();
	BuildColliderList(scene);

	for (int i = 0; i < m_nSteps; i++)
	{
		ResetStepState();
		CheckCollisions();

		if (m_intersections.empty())
			break;

		ResolveCollisions();
		SyncColliders();
	}
	
	ResolveCollisionForces();
	//InvokeCallbacks();
}

void CollisionHandler::Reset()
{
	ZoneScopedC(RandomUniqueColor());

	m_entitiesToCheck.clear();

	m_intersections.clear();
	m_entering.clear();
	m_exiting.clear();
}

void CollisionHandler::BuildColliderList(const Scene* scene)
{
	ZoneScopedC(RandomUniqueColor());

	//if (sh->GetRecalculateColliders())
	{
		std::vector<Entity *> entities;
		scene->GetEntities(entities);

		// Check if entities have colliders
		for (int i = 0; i < entities.size(); i++)
		{
			std::vector<Collisions::Collider> colliders;
			Entity *ent = entities[i];

			Collider *col = ent->GetCollider();
			if (col)
			{
				if (col->colliderType != NULL_COLLIDER)
				{
					// Add collider and entity to be checked
					m_entitiesToCheck.push_back(std::make_pair(ent, col));
				}
			}
		}
	}

	// Insertion sort
	{
		int j;
		for (int i = 1; i < m_entitiesToCheck.size(); i++)
		{

			j = i;
			while (j > 0 && m_entitiesToCheck[j - 1].second->GetMin().x > m_entitiesToCheck[j].second->GetMin().x)
			{
				std::swap(m_entitiesToCheck[j], m_entitiesToCheck[j - 1]);
				j--;
			}
		}
	}
}

void CollisionHandler::ResetStepState()
{
	ZoneScopedC(RandomUniqueColor());

	m_canResolve = false;
	m_intersections.clear();
}

void CollisionHandler::CheckCollisions()
{
    ZoneScopedC(RandomUniqueColor());

	CollisionData data;

	// Narrow phase

	{
		// TODO: Try makeing parallel solution work
		for (auto& ent1 : m_entitiesToCheck)
		{
			Collider *col1 = ent1.second;
			if (col1->GetCheck())
			{
				for (auto& ent2 : m_entitiesToCheck)
				{
					if (ent1 == ent2) continue;

					Collider* col2 = ent2.second;

					if (col2->GetMin().x > col1->GetMax().x) break;
					if (col2->GetMin().y > col1->GetMax().y || col1->GetMin().y > col2->GetMax().y) continue;
					if (col2->GetMin().z > col1->GetMax().z || col1->GetMin().z > col2->GetMax().z) continue;

					if (CheckIntersection(col1, col2, data))
					{
						ZoneScopedNC("Register Collision", RandomUniqueColor());
						data.other = col2;
						data.otherEntity = ent2.first;

						col1->SetIntersecting(true);

						m_intersections.insert({ ent1.first, data });

						//// Chnage data values
						data.normal = { -1 * data.normal.x, -1 * data.normal.y, -1 * data.normal.z };
						data.other = col1;
						data.otherEntity = ent1.first;

						// Other entity
						col2->SetIntersecting(true);
						m_intersections.insert({ ent2.first, data });
					}
				}

				if (!col1->GetIntersecting())
					col1->CheckDone();
			}
		}
	}

	m_canResolve = true;
}

void CollisionHandler::ResolveCollisions()
{
	ZoneScopedC(RandomUniqueColor());

	if (!m_canResolve  || m_intersections.empty())
		return;

	m_canResolve = false;

	const float stepFraction = 1.0f / (float)m_nSteps;
	for (auto const& [ent, data] : m_intersections)
	{
		// Make sure colliding entities have rigidbodies
		if (!ent->GetRigidbody())
			continue;

		float depth = std::max(data.depth - kSlop, 0.0f) / 2;
		if (data.otherEntity->GetRigidbody())
			depth /= 2.0f;

		float moveAmount = depth * kBias * stepFraction;
		ent->GetTransform().Move(data.normal, moveAmount);
	}
}

void CollisionHandler::SyncColliders()
{
	ZoneScopedC(RandomUniqueColor());

	for (auto const& [ent, data] : m_intersections)
		ent->TransformCollider();
}

void CollisionHandler::ResolveCollisionForces()
{
	ZoneScopedC(RandomUniqueColor());

	for (auto const& [ent, data] : m_intersections)
		if (ent->GetRigidbody())
			ent->GetRigidbody()->ResolveCollisionForces(data);
}

void CollisionHandler::InvokeCallbacks()
{
	ZoneScopedC(RandomUniqueColor());

	for (auto const &[ent, data] : m_intersections)
		ent->GetCollider()->Intersection(data);

	for (auto const &[ent, data] : m_entering)
		ent->GetCollider()->OnCollisionEnter(data);

	for (auto const &[ent, data] : m_exiting)
		ent->GetCollider()->OnCollisionExit();
}

