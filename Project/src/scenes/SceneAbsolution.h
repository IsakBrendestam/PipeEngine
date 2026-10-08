#pragma once

#include "core/scene/Scene.h"

class SceneAbsolution : public Scene
{
public:
	SceneAbsolution() : Scene("Absolution") {};
	~SceneAbsolution();

	void Load() override;
	void Unload() override;
	void Update() override;

	void InternalRenderUI() override;

private:
	ID m_ps = INVALID_ID;
};
