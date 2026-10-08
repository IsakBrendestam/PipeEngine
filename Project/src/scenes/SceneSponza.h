#pragma once

#include "core/scene/Scene.h"

class SceneSponza : public Scene
{
public:
	SceneSponza() : Scene("Sponza") { SetPhysicsEnabled(false); };
	~SceneSponza();

	void Load() override;
	void Unload() override;
	void Update() override;

	void InternalRenderUI() override;
};