#pragma once

#include "core/scene/Scene.h"

class SceneBenchmark : public Scene
{
public:
	SceneBenchmark() : Scene("Benchmark") { SetPhysicsEnabled(false); };
	~SceneBenchmark();

	void Load() override;
	void Unload() override;
	void Update() override;

	void PreRender() override;
	void PostRender() override;

	void InternalRenderUI() override;

private:
	void SaveBenchmarkData(const char* fileName);

	void Restart();

	bool DefaultBenchmark(unsigned int frame);
	bool SpecializedBenchmark(unsigned int frame);

	void CollectLastRenderStats();

private:
	enum BenchmarkStage
	{
		WAIT,
		DEFAULT,
		SPECIALIZED,
		DONE
	};

	
private:
	ID m_cs1 = INVALID_ID,
	   m_cs2 = INVALID_ID;

	Pipeline* m_defaultPipeline = nullptr,
			* m_specializedPipeline = nullptr;

	unsigned int m_interval = 250;
	const unsigned int m_waitFrames = 1000;

	bool m_running = false;

	BenchmarkStage m_stage = BenchmarkStage::WAIT;
	BenchmarkStage m_lastRunStage = BenchmarkStage::DONE;
	
	ID3D11Query* m_pipelineQuery = nullptr;
	std::vector<D3D11_QUERY_DATA_PIPELINE_STATISTICS> m_pipelineData;
};