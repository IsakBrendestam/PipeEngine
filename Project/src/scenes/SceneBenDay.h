#pragma once

#include "core/scene/Scene.h"

class SceneBenDay : public Scene
{
public:
	SceneBenDay() : Scene("BenDay") {};
	~SceneBenDay();

	void Load() override;
	void Unload() override;
	void Update() override;

	void InternalRenderUI() override;

private:
	ID m_cs = INVALID_ID;
};