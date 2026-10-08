#include "stdafx.h"
#include "ScopeTimer.h"

ScopeTimer::ScopeTimer(const std::string& name, bool average) :
	m_name(name),
	m_startTime(std::chrono::high_resolution_clock::now()),
	m_average(average)
{
	TracyD3D11ZoneC(g_tracyContext, "Timer Overhead", RandomUniqueColor());
}

ScopeTimer::~ScopeTimer()
{
	TracyD3D11ZoneC(g_tracyContext, "Timer Overhead", RandomUniqueColor());

	const auto endTime = std::chrono::high_resolution_clock::now();
	const double duration = std::chrono::duration<double, std::milli>(endTime - m_startTime).count();
	g_time.SetScopeTime(m_name, duration, m_average);
}
