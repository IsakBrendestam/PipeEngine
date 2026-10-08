#include "stdafx.h"
#include "Time.h"

unsigned int ScopeData::maxSamples = 1000;

Time g_time;

float Time::DeltaTime() const
{
	return static_cast<float>(m_deltaTime);
}

const double& Time::FPS() const
{
	return m_fps;
}

const double& Time::InitTime() const
{
	return m_initTime;
}

void Time::SetScopeTime(const std::string& name, double duration, bool average)
{
	auto& data = m_avgScopeTimes[name];
	if (!average)
	{
		data.empirical.clear();
		data.sum = 0;
	}

	data.sum += duration;

	data.empirical.push_back(duration);
	if (data.empirical.size() > ScopeData::maxSamples)
	{
		data.sum -= data.empirical.front();
		data.empirical.pop_front();
	}
}

double Time::GetScopeTime(const std::string& name) const
{
	auto it = m_avgScopeTimes.find(name);
    if (it == m_avgScopeTimes.end() || it->second.empirical.empty())
        return 0.0;

    const auto& data = it->second;
    return data.empirical.back();
}

double Time::GetAverageScopeTime(const std::string& name) const
{
	auto it = m_avgScopeTimes.find(name);
    if (it == m_avgScopeTimes.end() || it->second.empirical.empty())
        return 0.0;

    const auto& data = it->second;
    return data.sum / data.empirical.size();
}

const std::map<std::string, ScopeData>& Time::GetAllAverageScopeTimes() const
{
	return m_avgScopeTimes;
}

void Time::ResetAverageTimes()
{
	for (auto& it : m_avgScopeTimes)
	{
		it.second.empirical.clear();
		it.second.sum = 0.0;
	}
}
