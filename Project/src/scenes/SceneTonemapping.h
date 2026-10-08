#pragma once

#include "core/scene/Scene.h"

class SceneTonemapping : public Scene
{
public:
	SceneTonemapping() : Scene("Tonemapping") { SetPhysicsEnabled(false); };
	~SceneTonemapping();

	void Load() override;
	void Unload() override;
	void Update() override;

	void InternalRenderUI() override;
};