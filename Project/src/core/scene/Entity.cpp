#include "stdafx.h"
#include "Entity.h"

Entity::Entity():
	m_ID(UniqueID::GenerateID())
{
	m_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

Entity::~Entity()
{
	delete m_baseCollider;
	delete m_transformedCollider;
	delete m_rigidbody;
}

const bool Entity::operator<(const Entity& other) const
{
	if (m_mesh != other.m_mesh) return m_mesh < other.m_mesh;
	if (m_material != other.m_material) return m_material < other.m_material;
	return m_sampler < other.m_sampler;
}

const bool Entity::operator==(const Entity& other) const
{
	return m_ID == other.m_ID;
}

unsigned int Entity::GetID() const
{
	return m_ID;
}

void Entity::SetMesh(ID mesh)
{
	m_mesh = mesh;
}

ID Entity::GetMesh() const
{
	return m_mesh;
}

void Entity::SetMaterial(const MaterialMaps& material)
{
	m_material = material;
}

const MaterialMaps& Entity::GetMaterial() const
{
	return m_material;
}

void Entity::SetSampler(ID sampler)
{
	m_sampler = sampler;
}

ID Entity::GetSampler() const
{
	return m_sampler;
}

void Entity::SetTopology(D3D11_PRIMITIVE_TOPOLOGY topology)
{
	m_topology = topology;
}

D3D11_PRIMITIVE_TOPOLOGY Entity::GetTopology() const
{
	return m_topology;
}

void Entity::SetTransform(const Transform& transform)
{
	m_transform = transform;

	if (m_transform.IsDirty())
		TransformCollider();
}

Transform& Entity::GetTransform()
{
	return m_transform;
}

void Entity::SetCollider(Collisions::Collider* collider)
{
	if (!collider)
		return;

	delete m_baseCollider;
	m_baseCollider = collider;

	switch (m_baseCollider->colliderType)
	{
	case Collisions::RAY_COLLIDER:
		m_transformedCollider = new Collisions::Ray(*static_cast<Collisions::Ray *>(collider));
		break;
	case Collisions::SPHERE_COLLIDER:
		m_transformedCollider = new Collisions::Sphere(*static_cast<Collisions::Sphere *>(collider));
		break;
	case Collisions::CAPSULE_COLLIDER:
		m_transformedCollider = new Collisions::Capsule(*static_cast<Collisions::Capsule *>(collider));
		break;
	case Collisions::AABB_COLLIDER:
		m_transformedCollider = new Collisions::AABB(*static_cast<Collisions::AABB *>(collider));
		break;
	case Collisions::OBB_COLLIDER:
		m_transformedCollider = new Collisions::OBB(*static_cast<Collisions::OBB *>(collider));
		break;
	default:
		break;
	}

	TransformCollider();
}

Collisions::Collider* Entity::GetCollider() const
{
	return m_transformedCollider;
}

void Entity::SetRigidbody(Rigidbody* rigidbody)
{
	m_rigidbody = rigidbody;
}

Rigidbody* Entity::GetRigidbody() const
{
	return m_rigidbody;
}

void Entity::RenderUI(bool& changed, ResourceManager& resourceManager)
{
	m_transform.RenderUI();
	if (m_transform.IsDirty())
		TransformCollider();

	ImGui::Separator();

	if (m_rigidbody)
		m_rigidbody->RenderUI();

	ImGui::Separator();

	auto textureMapSelect = [&changed](ID& current, const char* category, const std::unordered_map<std::string, ID>& data, std::string& title) {
		if (ImGui::BeginCombo(category, title.c_str()))
		{
			for (const auto& [name, id] : data)
			{
				const bool isSelected = (id == current);
				if (ImGui::Selectable(std::format("{} (ID: {})", name, id).c_str(), isSelected))
				{
					title = std::format("{} (ID: {})", name, id);
					current = id;
					changed = true;
				}

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
	};

	// Mesh
	static std::string meshTitle = "No selected mesh";
	const auto& meshes = resourceManager.GetMeshes();
	textureMapSelect(m_mesh, "Mesh", meshes, meshTitle);

	const auto& textures = resourceManager.GetTextures();

	// Albedo
	static std::string albedoTitle = "No selected mesh";
	textureMapSelect(m_material.albedoMap, "Albedo Map", textures, albedoTitle);

	// Normal
	static std::string normalTitle = "No selected mesh";
	textureMapSelect(m_material.normalMap, "Normal Map", textures, normalTitle);

	// Ambient
	static std::string ambientTitle = "No selected mesh";
	textureMapSelect(m_material.ambientOcclusionMap, "Ambient Occlusion Map", textures, ambientTitle);
}

void Entity::Update()
{
	if (m_transform.IsDirty())
		TransformCollider();
}

void Entity::PhysicsUpdate(const double& stepSize)
{	
	if (m_rigidbody)
		m_rigidbody->ApplyForces(stepSize);

	if (m_transform.IsDirty())
		TransformCollider();
}

void Entity::TransformCollider()
{
	if (!m_baseCollider || !m_transformedCollider)
		return;

	m_baseCollider->Transform(DX::XMMatrixTranspose(m_transform.GetWorldMatrix()), m_transformedCollider);

#ifdef DEBUG_BUILD
		if (!m_transformedCollider->TransformDebug(GetScene()->GetContext(), time, input))
			Warn("Failed to transform collider debug!");
#endif
}
