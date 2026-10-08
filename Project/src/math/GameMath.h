#pragma once

#include <numbers>

namespace GameMath
{
	constexpr float DEG_TO_RAD = std::numbers::pi_v<float> / 180.0f;
	constexpr float RAD_TO_DEG = 180.0f / std::numbers::pi_v<float>;

	constexpr float EPSILON = 1e-6f;
}
