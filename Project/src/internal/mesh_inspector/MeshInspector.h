#pragma once

#include "core/rendering/Renderer.h"
#include "core/rendering/Pipeline.h"

#include "core/scene/Entity.h"

#include "assets/ResourceManager.h"

class MeshInspector
{
public:
	MeshInspector() = default;
	~MeshInspector();

	void Initialize();

	void Update();
	void Render(const ResourceManager& rm, Entity* entity);
	void RenderUI();

	void SetActive();

private:
	void SetEntity(const Entity* entity);
	bool CheckEntityChanged(const Entity* entity) const;

	void ChangeView();

private:
	Renderer m_renderer;
	Pipeline* m_pipeline = nullptr;
	Entity* m_entity = nullptr;

	RenderTargetD3D11 m_renderTarget;
	ResourceManager m_resourceManager;

	CameraD3D11 m_camera;

	ID m_vs = INVALID_ID,
	   m_ps = INVALID_ID;

	bool m_windowFocused = false;
	bool m_wasLocked = false;
	bool m_active = false;
};
