#pragma once

#include "core/scene/Scene.h"

class SceneGaussian : public Scene
{
public:
	SceneGaussian() : Scene("Gaussian") { SetPhysicsEnabled(false); };
	~SceneGaussian();

	void Load() override;
	void Unload() override;
	void Update() override;

	void InternalRenderUI() override;

	bool Benchmark(unsigned int frame, bool specialized);
};