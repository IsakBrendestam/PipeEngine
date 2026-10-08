#pragma once

#include "core/scene/Scene.h"

class SceneCollision : public Scene
{
public:
	SceneCollision() : Scene("Collision") {};
	~SceneCollision();

	void Load() override;
	void Unload() override;
	void Update() override;

	void InternalRenderUI() override;

private:
	void SpawnColliderEntity(const DX::XMFLOAT3& position, bool addImpulse = false);

	ID m_texGray = INVALID_ID,
	   m_texBlue = INVALID_ID,
	   m_texGreen = INVALID_ID,
	   m_texChecker = INVALID_ID,
	   m_meshCube = INVALID_ID,
	   m_sampler = INVALID_ID;

	Pipeline* m_pipeline = nullptr;

	float spawnRate = 0.4f;
	float spawnTime = 0;
};