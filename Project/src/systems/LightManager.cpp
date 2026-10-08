#include "stdafx.h"

#include "LightManager.h"

LightManager::~LightManager()
{
	Deconstruct();
}

void LightManager::Initialize(float ambientLight)
{
	ZoneScopedC(RandomUniqueColor());

	m_ambientLight = ambientLight;

	// Create Light Constant Buffers
	m_lightBufferData = { static_cast<unsigned int>(m_pointLights.size()), 
						  static_cast<unsigned int>(m_spotLights.size()),
						  static_cast<unsigned int>(m_directionalLights.size()),
						  m_ambientLight};
	m_lightBuffer = new ConstantBufferD3D11(sizeof(LightBuffer), true, &m_lightBufferData);

	// Create Individual Light Structured Buffers
	m_pointLightBuffer = new StructuredBufferD3D11(sizeof(PointLight), m_pointLights.size(), g_MAXLIGHTS, m_pointLights.data());
	m_spotLightBuffer = new StructuredBufferD3D11(sizeof(SpotLight), m_spotLights.size(), g_MAXLIGHTS, m_spotLights.data());
	m_directionalLigthBuffer = new StructuredBufferD3D11(sizeof(DirectionalLight), m_directionalLights.size(), g_MAXLIGHTS, m_directionalLights.data());
}

void LightManager::Deconstruct()
{
	ZoneScopedC(RandomUniqueColor());

	delete m_lightBuffer;

	for (int i = 0; i < m_spotLightCameras.size(); i++)
		delete m_spotLightCameras[i];

	for (auto& camera : m_directionalCameras)
		delete camera;

	delete m_pointLightBuffer;
	delete m_spotLightBuffer;
	delete m_directionalLigthBuffer;
}

void LightManager::UpdateBuffers()
{
	ZoneScopedC(RandomUniqueColor());

	// Loop over all spot lights
	for (int i = 0; i < m_spotLights.size(); i++)
	{
		// Update Camera
		m_spotLightCameras[i]->SetPosition(DX::XMFLOAT3(m_spotLights[i].base.pos));
		m_spotLightCameras[i]->SetForward(DX::XMFLOAT3(m_spotLights[i].dir));

		// Update Light
		m_spotLights[i].vp = m_spotLightCameras[i]->GetViewProjectionMatrix();
	}

	// Loop over all directional lights
	for (int i = 0; i < m_directionalLights.size(); i++)
	{
		// Update Camera
		m_directionalCameras[i]->SetPosition(DX::XMFLOAT3(m_directionalLights[i].base.pos));
		m_directionalCameras[i]->SetForward(DX::XMFLOAT3(m_directionalLights[i].dir));

		// Update Light
		m_directionalLights[i].vp = m_directionalCameras[i]->GetViewProjectionMatrix();
	}

	// Update Light Constant buffer
	m_lightBufferData = { static_cast<unsigned int>(m_pointLights.size()), 
						  static_cast<unsigned int>(m_spotLights.size()),
						  static_cast<unsigned int>(m_directionalLights.size()),
						  m_ambientLight};
	m_lightBuffer->UpdateBuffer(&m_lightBufferData);

	// Update Individual Light Structured Buffers
	if (m_pointLights.size() > 0)
		m_pointLightBuffer->UpdateBuffer(m_pointLights.data());

	if (m_spotLights.size() > 0)
		m_spotLightBuffer->UpdateBuffer(m_spotLights.data());

	if (m_directionalLights.size() > 0)
		m_directionalLigthBuffer->UpdateBuffer(m_directionalLights.data());
}

void LightManager::AddLight(PointLight light)
{
	// Check if Light limit is reached
	if (m_pointLights.size() < g_MAXLIGHTS)
	{
		m_pointLights.push_back(light);
		m_pointLightBuffer->ChangeElementCount(1);
	}
}

void LightManager::AddLight(SpotLight light)
{
	// Check if Light limit is reached
	if (m_spotLights.size() < g_MAXLIGHTS)
	{
		// Add Camera
		m_spotLightCameras.push_back(new CameraD3D11(m_defaultProjectionInfo));

		// Update Camera
		m_spotLightCameras[m_spotLights.size()]->SetPosition(DX::XMFLOAT3(light.base.pos));
		m_spotLightCameras[m_spotLights.size()]->SetForward(DX::XMFLOAT3(light.dir));

		// Set Light View Projection Matrix
		SpotLight tempLight = light;
		tempLight.vp = m_spotLightCameras[m_spotLights.size()]->GetViewProjectionMatrix();

		// Add Spot Light
		m_spotLights.push_back(tempLight);
		m_spotLightBuffer->ChangeElementCount(1);
	}
}

void LightManager::AddLight(DirectionalLight light)
{
	// Check if Light limit is reached
	if (m_directionalLights.size() < g_MAXLIGHTS)
	{
		// Add Camera
		m_directionalCameras.push_back(new CameraOrthographicD3D11({ DX::XM_PIDIV2, 1, 0.1f, 300 }, 100, 100));

		// Update Camera
		m_directionalCameras[m_directionalLights.size()]->SetPosition(DX::XMFLOAT3(light.base.pos));
		m_directionalCameras[m_directionalLights.size()]->SetForward(DX::XMFLOAT3(light.dir));

		// Set Light View Projection Matrix
		DirectionalLight tempLight = light;
		light.vp = m_directionalCameras[m_directionalLights.size()]->GetViewProjectionMatrix();

		// Add Directional Light
		m_directionalLights.push_back(tempLight);
		m_directionalLigthBuffer->ChangeElementCount(1);
	}
}

void LightManager::SetLight(int index, PointLight light)
{
	if (index < m_pointLights.size())
		m_pointLights[index] = light;
}

void LightManager::SetLight(int index, SpotLight light)
{
	if (index < m_spotLights.size())
		m_spotLights[index] = light;
}

void LightManager::SetLight(int index, DirectionalLight light)
{
	if (index < m_directionalLights.size())
		m_directionalLights[index] = light;
}

const PointLight& LightManager::GetPointLight(int index) const
{
	if (index < m_pointLights.size())
		return m_pointLights[index];
	return PointLight();
}

const SpotLight& LightManager::GetSpotLight(int index) const
{
	if (index < m_spotLights.size())
		return m_spotLights[index];
	return SpotLight();
}

const CameraD3D11* LightManager::GetSpotLightCamera(int index) const
{
	if (index < m_spotLightCameras.size())
		return m_spotLightCameras[index];
	return nullptr;
}

const DirectionalLight& LightManager::GetDirectionalLight(int index) const
{
	if (index < m_directionalLights.size())
		return m_directionalLights[index];
	return DirectionalLight();
}

const CameraOrthographicD3D11* LightManager::GetDirectionalCamera(int index) const
{
	if (index < m_directionalCameras.size())
		return m_directionalCameras[index];
	return nullptr;
}

unsigned int LightManager::GetPointLightCount() const
{
	return m_pointLights.size();
}

unsigned int LightManager::GetSpotLightCount() const
{
	return m_spotLights.size();
}

unsigned int LightManager::GetDirectinalLightCount() const
{
	return m_directionalLights.size();
}

void LightManager::SetGlobalAmbience(float ambience)
{
	m_ambientLight = ambience;
}

ID3D11Buffer* LightManager::GetBuffer()
{
	return m_lightBuffer->GetBuffer();
}

ID3D11ShaderResourceView* LightManager::GetPointLightSRV()
{
	return m_pointLightBuffer->GetSRV();
}

ID3D11ShaderResourceView* LightManager::GetSpotLightSRV()
{
	return m_spotLightBuffer->GetSRV();
}

ID3D11ShaderResourceView* LightManager::GetDirectionalLightSRV()
{
	return m_directionalLigthBuffer->GetSRV();
}

void LightManager::RenderUI()
{
	ZoneScopedC(RandomUniqueColor());

	ImGui::DragFloat("Ambient", &m_ambientLight, 0.01f, 0, 1000);

	if (ImGui::TreeNode("Point Lights"))
	{
		if (ImGui::Button("Add point light"))
			AddLight(PointLight());

		for (int i = 0; i < m_pointLights.size(); i++)
			if (ImGui::TreeNode(std::format("Point Light {}", i).c_str()))
			{
				PointLight& light = m_pointLights[i];
				ImGui::DragFloat3("Postion", light.pos, 0.01f, -1000, 1000);

				ImGui::DragFloat("Intencity", &light.I, 0.01f, 0, 1000);
				ImGui::DragFloat("Ambient", &light.Ka, 0.01f, 0, 1000);
				ImGui::DragFloat("Specular", &light.spec, 0.01f, 0, 1000);

				ImGui::ColorPicker3("Color", light.col);

				ImGui::TreePop();
			}

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Spot Lights"))
	{
		if (ImGui::Button("Add Spot light"))
			AddLight(SpotLight());

		for (int i = 0; i < m_spotLights.size(); i++)
			if (ImGui::TreeNode(std::format("Spot Light {}", i).c_str()))
			{
				SpotLight& light = m_spotLights[i];
				ImGui::DragFloat3("Postion", light.base.pos, 0.01f, -1000, 1000);
				ImGui::DragFloat3("Direction", light.dir, 0.01f, -1000, 1000);

				ImGui::DragFloat("Angle", &light.spot, 0.01f, 0, 1000);

				ImGui::DragFloat("Intencity", &light.base.I, 0.01f, 0, 1000);
				ImGui::DragFloat("Ambient", &light.base.Ka, 0.01f, 0, 1000);
				ImGui::DragFloat("Specular", &light.base.spec, 0.01f, 0, 1000);

				ImGui::ColorPicker3("Color", light.base.col);

				ImGui::TreePop();
			}

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Directional Lights"))
	{
		if (ImGui::Button("Add Directional light"))
			AddLight(DirectionalLight());

		for (int i = 0; i < m_directionalLights.size(); i++)
			if (ImGui::TreeNode(std::format("Directional Light {}", i).c_str()))
			{
				DirectionalLight& light = m_directionalLights[i];
				ImGui::DragFloat3("Postion", light.base.pos, 0.01f, -1000, 1000);
				ImGui::DragFloat3("Direction", light.dir, 0.01f, -1000, 1000);

				ImGui::DragFloat("Intencity", &light.base.I, 0.01f, 0, 1000);
				ImGui::DragFloat("Ambient", &light.base.Ka, 0.01f, 0, 1000);
				ImGui::DragFloat("Specular", &light.base.spec, 0.01f, 0, 1000);

				ImGui::ColorPicker3("Color", light.base.col);

				ImGui::TreePop();
			}

		ImGui::TreePop();
	}
}
