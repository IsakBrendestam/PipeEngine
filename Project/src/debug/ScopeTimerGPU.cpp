#include "stdafx.h"
#include "ScopeTimerGPU.h"

ID3D11Query* ScopeTimerGPU::m_disjointQuery = nullptr;
bool ScopeTimerGPU::m_initialized = false;
std::vector<TimerQueryGPU> ScopeTimerGPU::m_timerQueue;

ScopeTimerGPU::ScopeTimerGPU(const std::string& name):
	m_name(name)
{
	if (!m_initialized)
		return;

	TracyD3D11ZoneC(g_tracyContext, "Timer Overhead Start", RandomUniqueColor());
	D3D11_QUERY_DESC desc = {};

	// Timestamp queries
	desc.Query = D3D11_QUERY_TIMESTAMP;
	DeviceManager::Device()->CreateQuery(&desc, &m_timestampStart);
	DeviceManager::Device()->CreateQuery(&desc, &m_timestampEnd);

	DeviceManager::Context()->End(m_timestampStart);
}

ScopeTimerGPU::~ScopeTimerGPU()
{
	if (!m_initialized)
		return;

	TracyD3D11ZoneC(g_tracyContext, "Timer Overhead End", RandomUniqueColor());

	m_timerQueue.emplace_back(
		m_timestampStart,
		m_timestampEnd,
		m_name
	);
}


bool ScopeTimerGPU::TryGetData(TimerQueryGPU& data, const D3D11_QUERY_DATA_TIMESTAMP_DISJOINT& disjointData)
{
	ZoneScopedC(RandomUniqueColor());

	UINT64 startTime = 0;
	UINT64 endTime = 0;

	if (DeviceManager::Context()->GetData(data.timestampStart, &startTime, sizeof(UINT64), 0) != S_OK)
		return false;;
	if (DeviceManager::Context()->GetData(data.timestampEnd, &endTime, sizeof(UINT64), 0) != S_OK)
		return false;

	double duration = (double)(endTime - startTime) / disjointData.Frequency * 1000.0;

	g_time.SetScopeTime(data.name, duration, true);

	ReleaseCOM(data.timestampStart);
	ReleaseCOM(data.timestampEnd);

	return true;
}

void ScopeTimerGPU::PerFrame()
{
	ZoneScopedC(RandomUniqueColor());

	if (!m_initialized)
		return;

	DeviceManager::Context()->End(m_disjointQuery);

	D3D11_QUERY_DATA_TIMESTAMP_DISJOINT disjointData;
	if (DeviceManager::Context()->GetData(m_disjointQuery, &disjointData, sizeof(disjointData), D3D11_ASYNC_GETDATA_DONOTFLUSH) != S_OK)
		return;

	if (disjointData.Disjoint)
		return;

	for (int i = m_timerQueue.size() - 1; i >= 0; i--)
		if (TryGetData(m_timerQueue[i], disjointData))
			m_timerQueue.erase(m_timerQueue.begin() + i);

	DeviceManager::Context()->Begin(m_disjointQuery);
}

void ScopeTimerGPU::Initialize()
{
	D3D11_QUERY_DESC desc = {};

	// Disjoint query (for frequency + validity)
	desc.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
	DeviceManager::Device()->CreateQuery(&desc, &m_disjointQuery);
	DeviceManager::Context()->Begin(m_disjointQuery);

	m_initialized = true;
}

void ScopeTimerGPU::Deconstruct()
{
	ReleaseCOM(m_disjointQuery);
}


