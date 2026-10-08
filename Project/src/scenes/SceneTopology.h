#pragma once

#include "core/scene/Scene.h"

class SceneTopology : public Scene
{
public:
	SceneTopology() : Scene("Topology") {};
	~SceneTopology();

	void Load() override;
	void Unload() override;
	void Update() override;

	void InternalRenderUI() override;
};
