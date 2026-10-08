#pragma once

#include <d3d11.h>
#include <chrono>

#include <vector>

struct TimerQueryGPU
{
	ID3D11Query* timestampStart;
	ID3D11Query* timestampEnd;

	std::string name;
};

class ScopeTimerGPU
{
public:
	ScopeTimerGPU(const std::string& name);
	~ScopeTimerGPU();

	static bool TryGetData(TimerQueryGPU& data, const D3D11_QUERY_DATA_TIMESTAMP_DISJOINT& disjointData);
	static void PerFrame();

	static void Initialize();
	static void Deconstruct();

private:
	static ID3D11Query* m_disjointQuery;
	static bool m_initialized;
	static std::vector<TimerQueryGPU> m_timerQueue;

	ID3D11Query* m_timestampStart;
	ID3D11Query* m_timestampEnd;

	std::string m_name;
};

//#define InternalScopeGPU(name) ScopeTimerGPU InternalConcat(__ScopeTimer__, __LINE__)(name);
#define InternalScopeGPU(name) 
