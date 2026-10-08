#pragma once

#include "core/scene/Scene.h"

class SceneSobel : public Scene
{
public:
	SceneSobel() : Scene("Sobel") {};
	~SceneSobel();

	void Load() override;
	void Unload() override;
	void Update() override;

	void InternalRenderUI() override;
};