#pragma once

#include <chrono>

#include "MacroUtilities.h"

class ScopeTimer
{
public:
	ScopeTimer(const std::string& name, bool average = true);
	~ScopeTimer();

private:
	const std::string m_name;
	const std::chrono::steady_clock::time_point m_startTime;
	const bool m_average;
};

#define InternalScope(name) ScopeTimer InternalConcat(__ScopeTimer__, __LINE__)(name);
#define InternalScopeA(name, average) ScopeTimer  InternalConcat(__ScopeTimer__, __LINE__)(name, average);
