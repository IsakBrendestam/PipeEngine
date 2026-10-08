#pragma once

#include "core/rendering/d3d11/camera/CameraD3D11.h"

#include "Pipeline.h"
#include "core/scene/Entity.h"

class Renderer
{
public:
	Renderer();
	~Renderer();

	bool Initialize();

	void AddPipeline(Pipeline* pipeline);
	void RemovePipeline(unsigned int index);
	void RemovePipeline(Pipeline* pipeline);

	void Render(CameraD3D11* camera, ResourceManager* resourceManager);
	void Render(CameraD3D11* camera, ResourceManager* resourceManager, unsigned int index);

private:
	std::vector<Pipeline*> m_pipelines;

	ID3D11RenderTargetView* m_deaultRenderTarget = nullptr;
};