#pragma once

#include "assets/ResourceManager.h"

#include "core/rendering/Pipeline.h"

class PipelineUI
{
public:
	static void RenderUI(Pipeline* pipeline, ID& selectedEntity, Scene* currentScene, ResourceManager& resourceManager);

private:
	static bool ShaderOptions(Pipeline* pipeline, ResourceManager& resourceManager, const char* name, ShaderType type, ID &selectedShader);
	static bool ComputeShaderOptions(Pipeline* pipeline, ResourceManager& resourceManager, const char* name, ShaderType type, ID &selectedShader, int index);

	static bool SelectShader(ResourceManager& resourceManager, ShaderType type, ID &selectedShader);

	static bool OptimizationOption(ShaderOptimization& optimization, ID shaderID, ResourceManager& resourceManager);
	static void ShaderInfo(ShaderD3D11* shader);
	static bool ShaderResoures(ShaderD3D11* shader, Pipeline* pipeline, const ResourceManager& resourceManager, ID3D11ShaderResourceView*& texture);
	static void MacroOption(ShaderD3D11* shader);

	static void EntitiesOptions(Pipeline* pipeline, ID& selectedEntity, Scene* currentScene);
};
