#pragma once

#include "core/scene/Scene.h"

class SceneField : public Scene
{
public:
	SceneField() : Scene("Field") {};
	~SceneField();

	void Load() override;
	void Unload() override;
	void Update() override;

	void InternalRenderUI() override;
};