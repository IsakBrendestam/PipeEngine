#pragma once

#include <map>
#include <deque>

struct ScopeData {
	std::deque<double> empirical;
	double sum = 0.0;

	static unsigned int maxSamples;
};

static std::ostream& operator<<(std::ostream& os, const ScopeData& data)
{
	os << "{\n    \"data\": ";
	if (data.empirical.size() == 0)
		return os << "[]\n}";

	os << "[" << data.empirical[0];
	for (int i = 1; i < data.empirical.size(); i++)
		os << ", " << data.empirical[i];
	return os << "]\n}";
}

class Time
{
	friend class Engine;

public:

	template<typename T>
	T DeltaTime() const;
	float DeltaTime() const;

	const double& FPS() const;

	const double& InitTime() const;

	void SetScopeTime(const std::string& name, double duration, bool average);
	double GetScopeTime(const std::string& name) const;
	double GetAverageScopeTime(const std::string& name) const;
	const std::map<std::string, ScopeData>& GetAllAverageScopeTimes() const;

	void ResetAverageTimes();

private:
	double m_deltaTime = 0.0; // deltaTime in s
	double m_fps = 0.0;
	double m_initTime = 0.0; // time before first iteration of main loop in ms

	std::map<std::string, ScopeData> m_avgScopeTimes;
};

extern Time g_time;

template<typename T>
inline T Time::DeltaTime() const
{
	return static_cast<T>(m_deltaTime);
}
