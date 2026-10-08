#pragma once

#include <array>
#include <DirectXMath.h>

namespace DX = DirectX;

struct PointLight
{
	float pos[3];
	float Ka;
	float col[3];
	float I;
	float spec;
	float temp[3];

	PointLight()
	{
		Ka = I = spec = 0;
		for (int i = 0; i < 3; i++)
			pos[i] = col[i] = temp[i] = 0;
	}

	PointLight(const std::array<float, 3> position, const std::array<float, 3> color, const float intencity, const float ambient, const float specular):
		I(intencity), Ka(ambient), spec(specular)
	{
		for (int i = 0; i < 3; i++)
		{
			pos[i] = position[i];
			col[i] = color[i];
			temp[i] = 0;
		}
	}

	bool operator==(const PointLight& other)
	{
		bool equal = true;
		for (int i = 0; i < 3; i++)
		{
			if (pos[i] != other.pos[i])
				equal = false;
			if (col[i] != other.col[i])
				equal = false;
		}

		if (Ka != other.Ka)
			equal = false;
		if (I != other.I)
			equal = false;
		if (spec != other.spec)
			equal = false;

		return equal;
	}

	bool operator!=(const PointLight& other)
	{
		return !this->operator==(other);
	}
};

struct SpotLight
{
	PointLight base;
	float dir[3];
	float spot;
	DX::XMFLOAT4X4 vp;

	SpotLight()
	{
		base = PointLight();
		for (int i = 0; i < 3; i++)
			dir[i] = 0;
		spot = 0;

		float zeros[16] { 0 };
		vp = DX::XMFLOAT4X4(zeros);
	}

	SpotLight(const std::array<float, 3> position, const std::array<float, 3> color, const float intencity, const float ambient, const float specular,
		const std::array<float, 3> direction, float spotFactor) :
		base(PointLight(position, color, intencity, ambient, specular)), spot(spotFactor)
	{
		for (int i = 0; i < 3; i++)
		{
			dir[i] = direction[i];
		}

		float zeros[16] { 0 };
		vp = DX::XMFLOAT4X4(zeros);
	}

	bool operator==(const SpotLight& other)
	{
		for (int i = 0; i < 3; i++)
			if (dir[i] != other.dir[i])
				return false;

		if (spot != other.spot)
			return false;

		if (base != other.base)
			return false;

		return true;
	}

	bool operator!=(const SpotLight& other)
	{
		return !this->operator==(other);
	}
};

struct DirectionalLight
{
	PointLight base;
	float dir[3];
	float temp;
	DX::XMFLOAT4X4 vp;

	DirectionalLight()
	{
		base = PointLight();
		for (int i = 0; i < 3; i++)
			dir[i] = 0;

		float zeros[16]{ 0 };
		vp = DX::XMFLOAT4X4(zeros);
		temp = 0;
	}

	DirectionalLight(const std::array<float, 3> position, const std::array<float, 3> color, const float intencity, const float ambient, const float specular,
		const std::array<float, 3> direction) :
		base(PointLight(position, color, intencity, ambient, specular))
	{
		for (int i = 0; i < 3; i++)
		{
			dir[i] = direction[i];
		}

		float zeros[16]{ 0 };
		vp = DX::XMFLOAT4X4(zeros);
		temp = 0;
	}

	bool operator==(const SpotLight& other)
	{
		for (int i = 0; i < 3; i++)
			if (dir[i] != other.dir[i])
				return false;

		if (base != other.base)
			return false;

		return true;
	}

	bool operator!=(const SpotLight& other)
	{
		return !this->operator==(other);
	}
};