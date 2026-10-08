#pragma once


#include <DirectXMath.h>
#include <array>

namespace DX = DirectX;

struct ShaderVertex
{
	DX::XMFLOAT3 position;
	DX::XMFLOAT3 normal;
	DX::XMFLOAT2 uv;
	DX::XMFLOAT3 tangent;
	DX::XMFLOAT3 bitangent;
};

struct ShaderVertexNormalMap
{
	DX::XMFLOAT3 position;
	DX::XMFLOAT3 normal;
	DX::XMFLOAT3 tangent;
	DX::XMFLOAT3 bitangent;
	DX::XMFLOAT2 uv;

	ShaderVertexNormalMap() = default;

	ShaderVertexNormalMap(const std::array<float, 3>& pos,
						  const std::array<float, 3>& norm, 
						  const std::array<float, 3>& tan, 
						  const std::array<float, 3>& bitan, 
						  const std::array<float, 2> uvPosition)
	{
		position.x = pos[0];
		position.y = pos[1];
		position.z = pos[2];

		normal.x = norm[0];
		normal.y = norm[1];
		normal.z = norm[2];

		tangent.x = tan[0];
		tangent.y = tan[1];
		tangent.z = tan[2];

		bitangent.x = bitan[0];
		bitangent.y = bitan[1];
		bitangent.z = bitan[2];

		uv.x = uvPosition[0];
		uv.y = uvPosition[1];
	}

	bool operator==(const ShaderVertexNormalMap& other) const
	{
		return position.x == other.position.x && position.y == other.position.y && position.z == other.position.z &&
			   normal.x == other.normal.x && normal.y == other.normal.y && normal.z == other.normal.z &&
			   uv.x == other.uv.x && uv.y == other.uv.y;
	}

	std::size_t operator()(const ShaderVertexNormalMap& v) const {
		std::size_t h1 = std::hash<float>{}(v.position.x) ^
			std::hash<float>{}(v.position.y) ^
			std::hash<float>{}(v.position.z);
		std::size_t h2 = std::hash<float>{}(v.normal.x) ^
			std::hash<float>{}(v.normal.y) ^
			std::hash<float>{}(v.normal.z);
		std::size_t h3 = std::hash<float>{}(v.uv.x) ^
			std::hash<float>{}(v.uv.y);
		return h1 ^ (h2 << 1) ^ (h3 << 2);
	}
};

struct ShaderVertexUV
{
	DX::XMFLOAT3 position;
	DX::XMFLOAT3 normal;
	DX::XMFLOAT2 uv;

	ShaderVertexUV() = default;

	ShaderVertexUV(const std::array<float, 3>& pos,
						  const std::array<float, 3>& norm, 
						  const std::array<float, 2> uvPosition)
	{
		position.x = pos[0];
		position.y = pos[1];
		position.z = pos[2];

		normal.x = norm[0];
		normal.y = norm[1];
		normal.z = norm[2];

		uv.x = uvPosition[0];
		uv.y = uvPosition[1];
	}

	bool operator==(const ShaderVertexUV& other) const
	{
		return position.x == other.position.x && position.y == other.position.y && position.z == other.position.z &&
			   normal.x == other.normal.x && normal.y == other.normal.y && normal.z == other.normal.z &&
			   uv.x == other.uv.x && uv.y == other.uv.y;
	}

	std::size_t operator()(const ShaderVertexUV& v) const {
		std::size_t h1 = std::hash<float>{}(v.position.x) ^
			std::hash<float>{}(v.position.y) ^
			std::hash<float>{}(v.position.z);
		std::size_t h2 = std::hash<float>{}(v.normal.x) ^
			std::hash<float>{}(v.normal.y) ^
			std::hash<float>{}(v.normal.z);
		std::size_t h3 = std::hash<float>{}(v.uv.x) ^
			std::hash<float>{}(v.uv.y);
		return h1 ^ (h2 << 1) ^ (h3 << 2);
	}};

struct ShaderVertexColor
{
	float pos[3];
	float norm[3];
	float color[4] = { 1 };

	ShaderVertexColor(const std::array<float, 3>& position, const std::array<float, 3>& normal, const std::array<float, 4> col)
	{
		for (int i = 0; i < 3; i++)
		{
			pos[i] = position[i];
			norm[i] = normal[i];
		}

		for (int i = 0; i < 4; i++)
			color[i] = col[i];
	}
};