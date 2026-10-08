#include "stdafx.h"
#include "Renderer.h"

Renderer::Renderer()
{

}

Renderer::~Renderer()
{

}

bool Renderer::Initialize()
{
	m_deaultRenderTarget = DeviceManager::RenderRTV()->GetRTV();

	return true;
}

void Renderer::AddPipeline(Pipeline* pipeline)
{
	m_pipelines.push_back(pipeline);
}

void Renderer::RemovePipeline(unsigned int index)
{
	m_pipelines.erase(m_pipelines.begin() + index);
}

void Renderer::RemovePipeline(Pipeline* pipeline)
{
	auto pos = std::find(m_pipelines.begin(), m_pipelines.end(), pipeline);
	if (pos != m_pipelines.end())
		m_pipelines.erase(pos);
}

void Renderer::Render(CameraD3D11* camera, ResourceManager* resourceManager)
{
	DeviceManager::Context()->ClearRenderTargetView(m_deaultRenderTarget, g_engineSettings.clearColor);
	for (auto& pipeline : m_pipelines)
		pipeline->ClearRtv();

	for (auto& pipeline : m_pipelines)
	{
		if (pipeline->IsActive())
		{
			pipeline->StartRender(resourceManager);
			pipeline->Render(camera);
			pipeline->EndRender();
		}
	}
}

void Renderer::Render(CameraD3D11* camera, ResourceManager* resourceManager, unsigned int index)
{
	if (index >= m_pipelines.size()) return;

	m_pipelines[index]->StartRender(resourceManager);
	m_pipelines[index]->Render(camera);
}
