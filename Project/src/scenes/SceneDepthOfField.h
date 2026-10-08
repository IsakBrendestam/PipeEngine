#pragma once

#include "core/scene/Scene.h"

class SceneDepthOfField : public Scene
{
public:
	SceneDepthOfField() : Scene("DepthOfField") {};
	~SceneDepthOfField();

	void Load() override;
	void Unload() override;
	void Update() override;

	void InternalRenderUI() override;
};
