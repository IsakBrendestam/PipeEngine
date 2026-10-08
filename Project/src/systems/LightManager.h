#pragma once

#include <array>

#include "core/graphics/Light.h"

#include "core/rendering/d3d11/buffers/ConstantBufferD3D11.h"
#include "core/rendering/d3d11/buffers/StructuredBufferD3D11.h"
#include "core/rendering/d3d11/camera/CameraOrthographicD3D11.h"

#include "systems/CameraManager.h"

class LightManager
{
public:
	LightManager() = default;
	~LightManager();

	void Initialize(float ambientLight);
	void Deconstruct();

	void UpdateBuffers();

	void AddLight(PointLight light);
	void AddLight(SpotLight light);
	void AddLight(DirectionalLight light);

	void SetLight(int index, PointLight light);
	void SetLight(int index, SpotLight light);
	void SetLight(int index, DirectionalLight light);

	const PointLight& GetPointLight(int index) const;
	const SpotLight& GetSpotLight(int index) const;
	const CameraD3D11* GetSpotLightCamera(int index) const;
	const DirectionalLight& GetDirectionalLight(int index) const;
	const CameraOrthographicD3D11* GetDirectionalCamera(int index) const;

	unsigned int GetPointLightCount() const;
	unsigned int GetSpotLightCount() const;
	unsigned int GetDirectinalLightCount() const;

	void SetGlobalAmbience(float ambience);

	ID3D11Buffer* GetBuffer();

	ID3D11ShaderResourceView* GetPointLightSRV();
	ID3D11ShaderResourceView* GetSpotLightSRV();
	ID3D11ShaderResourceView* GetDirectionalLightSRV();

	void RenderUI();

	static constexpr UINT g_MAXLIGHTS = 25;		// ** Have to be matched with value int HLSL **

private:
	struct LightBuffer
	{
		int numPointLights;
		int numSpotLights;
		int numDirectionalLights;
		float ambinetLight;

		LightBuffer()
		{
			numPointLights = 0;
			numSpotLights = 0;
			numDirectionalLights = 0;
			ambinetLight = 0;
		}

		LightBuffer(unsigned int nPointLights, unsigned int nSpotLights, unsigned int nDirectionalLights, float ambient) :
			numPointLights(nPointLights), numSpotLights(nSpotLights), numDirectionalLights(nDirectionalLights), ambinetLight(ambient)
		{
		}
	};

	LightBuffer m_lightBufferData = {};

	std::vector<PointLight> m_pointLights;

	// m_spotLights and m_spotLightCameras are linked
	std::vector<SpotLight> m_spotLights;
	std::vector<CameraD3D11*> m_spotLightCameras;

	// m_directionalLights and m_directionalCameras are linked
	std::vector<DirectionalLight> m_directionalLights;
	std::vector<CameraOrthographicD3D11*> m_directionalCameras;

	float m_ambientLight = 0.0f;

	ConstantBufferD3D11* m_lightBuffer = nullptr;

	StructuredBufferD3D11* m_spotLightBuffer = nullptr;
	StructuredBufferD3D11* m_pointLightBuffer = nullptr;
	StructuredBufferD3D11* m_directionalLigthBuffer = nullptr;

	const ProjectionInfo m_defaultProjectionInfo;
};