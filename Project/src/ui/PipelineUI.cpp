#include "stdafx.h"

#include <format>

#include "PipelineUI.h"
#include "core/scene/Scene.h"

void PipelineUI::RenderUI(Pipeline* pipeline, ID& selectedEntity, Scene* currentScene, ResourceManager& resourceManager)
{
	ZoneScopedC(RandomUniqueColor());

	ImGui::Checkbox("Active", &pipeline->m_active);
	ImGui::Checkbox("Clear Output", &pipeline->m_clearRenderTarget);

	if (ImGui::TreeNode("Shaders"))
	{		
		ShaderOptions(pipeline, resourceManager, "Vertex Shaders", ShaderType::VERTEX_SHADER, pipeline->m_vShaderID);
		ShaderOptions(pipeline, resourceManager, "Pixel Shaders", ShaderType::PIXEL_SHADER, pipeline->m_pShaderID);
		for (int i = 0; i < pipeline->m_cShaderIDs.size(); i++)
			ComputeShaderOptions(pipeline, resourceManager, std::format("Compute Shader##{}", i).c_str(), ShaderType::COMPUTE_SHADER, pipeline->m_cShaderIDs[i], i);
	
		if (ImGui::Button("Add compute shader"))
			pipeline->SetShader(INVALID_ID, ShaderType::COMPUTE_SHADER);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Entities"))
	{
		EntitiesOptions(pipeline, selectedEntity, currentScene);
	}

	if (ImGui::TreeNode("Lights"))
	{
		pipeline->m_lightManager.RenderUI();

		ImGui::TreePop();
	}
}

bool PipelineUI::ShaderOptions(Pipeline* pipeline, ResourceManager& resourceManager, const char* name, ShaderType type, ID& selectedShader)
{
	if (ImGui::TreeNode(name))
	{
		if (SelectShader(resourceManager, type, selectedShader))
			if (selectedShader == INVALID_ID)
				pipeline->RemoveShader(selectedShader, type);

		ShaderD3D11* shader = resourceManager.GetShaderMutable(selectedShader);
		if (shader)
		{
			if (ImGui::Button("Recompile"))
				shader->ForceRecompile();

			ImGui::SameLine();

			if (ImGui::Button("Add Macro"))
				ImGui::OpenPopup("AddMacroPopup");

			ShaderOptimization opt;
			if (OptimizationOption(opt, selectedShader, resourceManager))
				shader->SetOptimization(opt);

			ShaderInfo(shader);

			MacroOption(shader);
		}

		ImGui::TreePop();
	}

	return false;
}

bool PipelineUI::ComputeShaderOptions(Pipeline* pipeline, ResourceManager& resourceManager, const char* name, ShaderType type, ID& selectedShader, int index)
{
	const bool open = ImGui::TreeNodeEx(name, ImGuiTreeNodeFlags_SpanLabelWidth);
	ImGui::SameLine();

	const int count = static_cast<int>(pipeline->m_cShaderIDs.size());

	// Determine which buttons to show
	const bool showUp   = (index > 0);
	const bool showDown = (index < count - 1);
	const bool showDel  = true;

	// Count buttons for layout
	int buttonCount = 3; // Always 3 slots for consistent alignment
	const float buttonWidth = 20.0f;
	const float spacing = ImGui::GetStyle().ItemSpacing.x;
	const float totalWidth = buttonCount * buttonWidth + (buttonCount - 1) * spacing;

	// Calculate right aligned starting position
	const float rightEdge = ImGui::GetWindowContentRegionMax().x;
	ImGui::SetCursorPosX(rightEdge - totalWidth);
	ImGui::AlignTextToFramePadding();

	// Helper to place buttons on same line
	bool first = true;
	auto SameLineIfNeeded = [&first]()
	{
		if (!first)
			ImGui::SameLine();
		first = false;
	};

	// Up button
	if (showUp)
	{
		SameLineIfNeeded();
		if (ImGui::ArrowButton(std::format("##up{}{}", name, index).c_str(), ImGuiDir_Up))
		{
			std::swap(pipeline->m_cShaderIDs[index], pipeline->m_cShaderIDs[index - 1]);
			std::swap(pipeline->m_cTextures[index], pipeline->m_cTextures[index - 1]);
		}
	}
	else
	{
		// Invisible placeholder for alignment
		SameLineIfNeeded();
		ImGui::Dummy(ImVec2(buttonWidth, 0));
	}

	// Down button
	if (showDown)
	{
		SameLineIfNeeded();
		if (ImGui::ArrowButton(std::format("##down{}{}", name, index).c_str(), ImGuiDir_Down))
		{
			std::swap(pipeline->m_cShaderIDs[index], pipeline->m_cShaderIDs[index + 1]);
			std::swap(pipeline->m_cTextures[index], pipeline->m_cTextures[index + 1]);
		}
	}
	else
	{
		// Invisible placeholder to keep alignment for last item
		SameLineIfNeeded();
		ImGui::Dummy(ImVec2(buttonWidth, 0));
	}

	// Delete button (always visible)
	SameLineIfNeeded();
	if (ImGui::Button(std::format("X##delete{}{}", name, index).c_str(), ImVec2(buttonWidth, 0)))
		pipeline->m_cShaderIDs.erase(pipeline->m_cShaderIDs.begin() + index);

	// Check if the tree node is open
	if (open)
	{
		if (SelectShader(resourceManager, type, selectedShader))
			if (selectedShader == INVALID_ID)
				pipeline->RemoveShader(selectedShader, type);

		ShaderD3D11* shader = resourceManager.GetShaderMutable(selectedShader);
		if (shader)
		{
			if (ImGui::Button("Recompile"))
				shader->ForceRecompile();

			ImGui::SameLine();

			if (ImGui::Button("Add Macro"))
				ImGui::OpenPopup("AddMacroPopup");

			ShaderOptimization opt;
			if (OptimizationOption(opt, selectedShader, resourceManager))
				shader->SetOptimization(opt);

			ShaderInfo(shader);

			ID3D11ShaderResourceView* texture = nullptr;
			if (ShaderResoures(shader, pipeline, resourceManager, texture));
				if (texture)
					pipeline->AddTexture(texture, ShaderType::COMPUTE_SHADER, index);

			MacroOption(shader);
		}

		ImGui::TreePop();
	}

	return false;
}

bool PipelineUI::SelectShader(ResourceManager& resourceManager, ShaderType type, ID& selectedShader)
{
	bool changed = false;

	const auto& shaders = resourceManager.GetShaderIDs();
	std::vector<std::pair<ID, std::string>> shaderIDs;
	shaderIDs.reserve(shaders.size());
	for (const auto& [name, id] : shaders)
		shaderIDs.emplace_back(id, name);

	shaderIDs.emplace_back(INVALID_ID, "None");

	std::string title = "No selected shader of this type";
	for (int i = 0; i < shaderIDs.size() - 1; i++)
		if (resourceManager.GetShader(shaderIDs[i].first)->GetType() == type &&
			shaderIDs[i].first == selectedShader)
			title = std::format("{} (ID: {})", shaderIDs[i].second, shaderIDs[i].first);

	if (ImGui::BeginCombo("Shader", title.c_str()))
	{
		for (int i = 0; i < shaderIDs.size(); i++)
		{
			const ID selectedID = shaderIDs[i].first;
			const bool isSelected = (selectedID == selectedShader);

			if (selectedID != INVALID_ID)
				if (resourceManager.GetShader(selectedID)->GetType() != type)
					continue;

			if (ImGui::Selectable(std::format("{} (ID: {})", shaderIDs[i].second, selectedID).c_str(), isSelected))
			{
				selectedShader = selectedID;
				changed = true;
			}

			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	return changed;
}

bool PipelineUI::OptimizationOption(ShaderOptimization& optimization, ID shaderID, ResourceManager& resourceManager)
{
	bool changed = false;

	int currentOptimization = (int)resourceManager.GetShader(shaderID)->GetOptimization();
	std::string title = std::format("O{}", currentOptimization);

	if (ImGui::BeginCombo("Optimization", title.c_str()))
	{
		for (int i = 0; i < (int)ShaderOptimization::OPTIMIZATION_COUNT; i++)
		{
			const bool isSelected = (i == currentOptimization);

			if (ImGui::Selectable(std::format("O{}", i).c_str(), isSelected))
			{
				optimization = (ShaderOptimization)i;
				changed = true;
			}

			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	return changed;
}

void PipelineUI::ShaderInfo(ShaderD3D11* shader)
{
	if (ImGui::TreeNode("Info"))
	{
		ShaderInformation info = shader->Reflection();

		for (const auto& res : info.resources)
			if (ImGui::TreeNode(res.name.c_str()))
			{
				const char* t = g_shaderInputTypeStringMap.at(res.type);
				ImGui::Text(std::format("Type: {}", t).c_str());
				ImGui::Text(std::format("Bind Point: {}", res.bindPoint).c_str());
				ImGui::Text(std::format("Bind Count: {}", res.bindCount).c_str());
				ImGui::Text(std::format("Flags: {}", res.flags).c_str());

				ImGui::TreePop();
			}

		ImGui::TreePop();
	}
}

bool PipelineUI::ShaderResoures(ShaderD3D11* shader, Pipeline* pipeline, const ResourceManager& resourceManager, ID3D11ShaderResourceView*& texture)
{
	bool changed = false;
	ShaderInformation info = shader->Reflection();

	static std::string title;
	static ID3D11ShaderResourceView* current = nullptr;

	ImGui::SeparatorText("Resources");

	for (const auto& res : info.resources)
	{
		if (res.type == D3D_SIT_TEXTURE)
		{
			if (ImGui::BeginCombo(res.name.c_str(), title.c_str()))
			{
				bool isSelected = false;

				ID3D11ShaderResourceView* temp = nullptr;

				temp = pipeline->GetBackBuffer();
				isSelected = (temp == current);
				if (isSelected)
					title = "Back Buffer";
				if (ImGui::Selectable("Back Buffer", isSelected))
				{
					title = "Back Buffer";
					current = temp;
					changed = true;
				}

				temp = pipeline->GetDepthBuffer();
				isSelected = (temp == current);
				if (isSelected)
					title = "Depth Buffer";
				if (ImGui::Selectable("Depth Buffer", isSelected))
				{
					title = "Depth Buffer";
					current = temp;
					changed = true;
				}


				if (!changed)
				{
					for (const auto& [name, id] : resourceManager.GetTextures())
					{
						temp = resourceManager.GetTexture(id)->GetSRV();
						bool isSelected = (temp == current);
						std::string text = std::format("{} (ID: {})", name, id);

						if (isSelected)
							title = text;

						if (ImGui::Selectable(text.c_str(), isSelected))
						{
							title = text;
							current = temp;
							changed = true;
							break;
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
				}

				ImGui::EndCombo();
			}
		}
	}

	texture = current;
	return changed;
}

void PipelineUI::MacroOption(ShaderD3D11* shader)
{
	static std::vector<ShaderMacro> macros = shader->GetMacros();

	ImGui::SeparatorText("Macros");

	if (ImGui::BeginPopupModal("AddMacroPopup", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		static char name[64] = "", value[64] = "";
		ImGui::InputText("Name", name, IM_ARRAYSIZE(name));
		ImGui::InputText("Value", value, IM_ARRAYSIZE(value));

		if (ImGui::Button("Done"))
		{
			shader->SetMacroValue(name, value);
			shader->Recompile();
			macros = shader->GetMacros();
			memset(name, '\0', 64);
			memset(value, '\0', 64);
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::Button("Cancel"))
		{
			memset(name, '\0', 64);
			memset(value, '\0', 64);
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	for (int i = 0; i < macros.size(); i++)
	{
		auto& macro = macros[i];

		ImGui::Text(macro.name, ":");
		ImGui::SameLine();

		bool changed = ImGui::InputText(std::format("##{}", i).c_str(), macro.value, IM_ARRAYSIZE(macro.value), ImGuiInputTextFlags_EnterReturnsTrue);
		changed |= ImGui::IsItemDeactivatedAfterEdit();
		if (changed)
		{
			shader->SetMacroValue(macro.name, macro.value);
			shader->Recompile();
		}
	}
}

void PipelineUI::EntitiesOptions(Pipeline* pipeline, ID& selectedEntity, Scene* currentScene)
{
	if (ImGui::Button("Create"))
	{
		Entity* ent = currentScene->CreateEntity();
		pipeline->AddEntity(ent);
		selectedEntity = ent->GetID();
	}

	ImGui::SameLine();

	if (ImGui::Button("Remove"))
	{
		currentScene->RemoveEntity(selectedEntity);
		selectedEntity = INVALID_ID;
	}

	for (int i = 0; i < pipeline->m_entities.size(); i++)
	{
		ID id = pipeline->m_entities[i]->GetID();
		std::string title = std::format("Entity (ID: {})", id);

		if (ImGui::Selectable(title.c_str(), selectedEntity == id))
			selectedEntity = id;
	}

	ImGui::TreePop();
}
