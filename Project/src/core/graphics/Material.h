#pragma once

#include "core/UniqueID.h"

extern ID g_fallbackTex;
extern ID g_defaultNormalTex;
extern ID g_defaultAmbientOcclusionTex;

struct MaterialMaps
{
	ID albedoMap = g_fallbackTex;
	ID normalMap = g_defaultNormalTex;
	ID ambientOcclusionMap = g_defaultAmbientOcclusionTex;
	ID roughnessMap = g_defaultAmbientOcclusionTex;
	ID metalnessMap = g_defaultAmbientOcclusionTex;

	//MaterialMaps();
	//MaterialMaps(ID albedo);
	//MaterialMaps(ID albedo, ID normal);
	//MaterialMaps(ID albedo, ID normal, ID ambientOcclusion);

	const inline bool operator<(const MaterialMaps& other) const
	{
		if (albedoMap != other.albedoMap) return albedoMap < other.albedoMap;
		if (normalMap != other.normalMap) return normalMap < other.normalMap;
		if (ambientOcclusionMap != other.ambientOcclusionMap) return ambientOcclusionMap < other.ambientOcclusionMap;
		if (roughnessMap != other.roughnessMap) return roughnessMap < other.roughnessMap;
		return metalnessMap < other.metalnessMap;
	}

	const inline bool operator==(const MaterialMaps& other) const
	{
		return albedoMap == other.albedoMap &&
			   normalMap == other.normalMap &&
			   ambientOcclusionMap == other.ambientOcclusionMap &&
			   roughnessMap == other.roughnessMap &&
			   metalnessMap == other.metalnessMap;
	}
};

static MaterialMaps g_nullMaterialMap = MaterialMaps(INVALID_ID, INVALID_ID, INVALID_ID);

struct Material
{
	float amb;
	float diff;
	float spec;
	float shine;

	Material():
		amb(0), diff(0), spec(0), shine(0)
	{
	}

	Material(const float ambient, const float diffuse, const float specular, const float shininess):
		amb(ambient), diff(diffuse), spec(specular), shine(shininess)
	{
	}
};