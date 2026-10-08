#pragma once


#include <Windows.h>
#include <d3d11_4.h>
#include <iostream>
#include <sstream>
#include <ctime>
#include <string>

#ifdef _DEBUG
#ifndef DEBUG
#define DEBUG true
#endif
#else
#ifndef DEBUG
#define DEBUG false
#endif
#endif // !_DEBUG

enum DebugViewMode : uint32_t
{
    Debug_None = 0,
    Debug_Normals,
    Debug_Tangents,
    Debug_Bitangents,
    Debug_UV0,
    Debug_AO,
    Debug_Albedo,
    Debug_Depth,
};

#ifdef _DEBUG
void DebugLog(std::string x);
#else
#define DebugLog(_)
#endif

#define VariableName(__var__) std::string(#__var__)

#define RandomUniqueColor() (const uint32_t)ConstRand(sizeof(__FILE__) * (const size_t)__LINE__, (const size_t)0x000000, (const size_t)0xFFFFFF)

// This is a simple implementation of a linear congruential generator (LCG).
constexpr size_t ConstRand(const size_t seed, const size_t min, const size_t max)
{
	// Constants for the LCG
	constexpr size_t a = 1664525;
	constexpr size_t c = 1013904223;
	constexpr size_t m = 4294967296;

	// Generate the next random number
	const size_t rnd = (a * seed + c) % m;

	// Scale the result to the desired range
	return min + (rnd % (max - min));
}

void UpdateError();
