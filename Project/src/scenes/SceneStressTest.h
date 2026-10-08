#pragma once

#include "core/scene/Scene.h"
#include "math/Random.h"

class SceneStressTest : public Scene
{
public:
	SceneStressTest() : Scene("StressTest") {};
	~SceneStressTest();

	void Load() override;
	void Unload() override;
	void Update() override;

	void InternalRenderUI() override;

private:
	void AddRandomEntity(Pipeline* pipeline);

	Random m_random = Random();

	std::array<ID, 7> meshes	= { INVALID_ID };
	std::array<ID, 9> textures	= { INVALID_ID };
	std::array<ID, 5> normals	= { INVALID_ID };
	std::array<ID, 5> aos		= { INVALID_ID };
	std::array<ID, 1> samplers	= { INVALID_ID };
};
