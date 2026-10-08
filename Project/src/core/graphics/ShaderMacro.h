#pragma once
#include <string>
#include <vector>

struct ShaderMacro
{
	char name[64], value[64];

    bool operator==(const ShaderMacro& other) const {
        return strcmp(name, other.name) == 0 && strcmp(value, other.value) == 0;
    }
};

struct ShaderMacroHash {
    std::size_t operator()(const ShaderMacro& s) const {
        std::size_t h1 = std::hash<std::string>{}(s.name);
        std::size_t h2 = std::hash<std::string>{}(s.value);
        return h1 ^ (h2 << 1); // combine hashes
    }
};

struct MacroVectorHash {
    std::size_t operator()(const std::vector<ShaderMacro>& vec) const {
        std::size_t seed = 0;
        ShaderMacroHash hasher;

        for (const auto& v : vec) {
            seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }

        return seed;
    }
};