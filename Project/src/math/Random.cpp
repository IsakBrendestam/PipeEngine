#include "stdafx.h"
#include "Random.h"

#include <random>


Random::Random()
{
    srand(time(nullptr));
}

int Random::Range(int min, int max)
{
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}
