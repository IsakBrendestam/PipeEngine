#pragma once

#include "core/scene/Scene.h"

class SceneCTR : public Scene
{
public:
	SceneCTR() : Scene("CTR") {};
	~SceneCTR();

	void Load() override;
	void Unload() override;
	void Update() override;

	void InternalRenderUI() override;

private:
	ID m_cs = INVALID_ID;
};