#pragma once

#include "core/scene/Scene.h"

class SceneBloom : public Scene
{
public:
	SceneBloom() : Scene("Bloom") { SetPhysicsEnabled(false); };
	~SceneBloom();

	void Load() override;
	void Unload() override;
	void Update() override;

	void InternalRenderUI() override;
};