#pragma once

#include <vector>
#include "core/rendering/Renderer.h"
#include "core/rendering/Pipeline.h"

#include "core/scene/Entity.h"

#include "internal/mesh_inspector/MeshInspector.h"

#include "systems/LightManager.h"

class Scene
{
public:
	Scene(const std::string& name) : m_name(name) {};
	virtual ~Scene();

	void Initialize();

	void ExternalUnload();
	void ExternalLoad();
	void ExternalUpdate();
	void InternalPhysicsUpdate(const double& stepSize);

	virtual void PreRender() {};
	virtual void Render(CameraD3D11 *camera);
	virtual void PostRender() {};

	void RenderUI();

	inline const std::string& GetName() const { return m_name; }

	Entity* CreateEntity();	
	void RemoveEntity(ID entity);
	void RemoveEntity(Entity* entity);
	void GetEntities(std::vector<Entity*>& entities) const;

	void AddCamera();

	Pipeline* CreatePipeline();
	
	void OpenMeshInspector();

	bool PhysicsEnabled() const;

protected:
	virtual void Update() = 0;

	virtual void Load() = 0;
	virtual void Unload() = 0;

	virtual void InternalRenderUI() = 0;

	void SetPhysicsEnabled(bool enabled);

	Pipeline* GetPipeline(unsigned int index);
	const CameraD3D11* GetActiveCamera() const;

protected:
	Renderer m_renderer;
	ResourceManager m_resourceManager;

private:
	std::vector<Pipeline*> m_pipelines;
	std::vector<Entity*> m_entities;
	std::vector<ID> m_cameras;

	ID m_selectedEntity = INVALID_ID;

	std::string m_name;

	MeshInspector m_meshInspector;

	bool m_physicsEnabled = true;
};