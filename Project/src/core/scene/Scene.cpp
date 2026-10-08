#include "stdafx.h"
#include "Scene.h"

#include "ui/PipelineUI.h"

Scene::~Scene()
{
    for (auto& pipeline : m_pipelines)
        delete pipeline;

    for (auto& entity : m_entities)
        delete entity;
}

void Scene::Initialize()
{
	ZoneScopedC(RandomUniqueColor());

	m_renderer.Initialize();
	m_meshInspector.Initialize();
}

void Scene::ExternalUnload()
{
	ZoneScopedC(RandomUniqueColor());

	Unload();
	m_resourceManager.Deallocate();
	
	for (auto& pipeline : m_pipelines)
	{
		m_renderer.RemovePipeline(pipeline);
        delete pipeline;
	}
	m_pipelines.clear();

    for (auto& entity : m_entities)
        delete entity;
	m_entities.clear();

	for (auto& cameraId : m_cameras)
		g_cameraManager.RemoveCamera(cameraId);
	m_cameras.clear();
}

void Scene::ExternalLoad()
{
	ZoneScopedC(RandomUniqueColor());

	m_resourceManager = g_resourceManager;
	AddCamera();
	g_cameraManager.SetCurrentCamera(m_cameras[0]);
	g_cameraManager.SetControllCamera(m_cameras[0]);
	Load();
}

void Scene::ExternalUpdate()
{
	ZoneScopedC(RandomUniqueColor());

	m_meshInspector.Update();
	Update();
}

void Scene::InternalPhysicsUpdate(const double& stepSize)
{
	ZoneScopedC(RandomUniqueColor());

	for (auto& entity : m_entities)
		if (entity->GetRigidbody())
			entity->PhysicsUpdate(stepSize);
}

void Scene::Render(CameraD3D11* camera)
{
    ZoneScopedC(RandomUniqueColor());
	PreRender();

	std::vector<Entity*>::iterator selected = std::find_if(m_entities.begin(), m_entities.end(), [this](Entity* ent) { 
		return ent->GetID() == m_selectedEntity; 
	});
	Entity* selectedEntity = (selected != m_entities.end()) ? *selected : nullptr;
	m_meshInspector.Render(m_resourceManager, selectedEntity);

    m_renderer.Render(camera, &m_resourceManager);

	PostRender();
}

void Scene::RenderUI()
{
	ZoneScopedC(RandomUniqueColor());

	{
		ZoneScopedNC("Scene", RandomUniqueColor());

		ImGui::Begin("Scene");
		{

			for (int i = 0; i < m_pipelines.size(); i++)
				if (ImGui::TreeNode(std::format("Pipeline {}", i).c_str()))
				{
					PipelineUI::RenderUI(m_pipelines[i], m_selectedEntity, this, m_resourceManager);
					ImGui::TreePop();
				}

			ImGui::End();
		}
	}

	{
		ZoneScopedNC("Entity Inspector", RandomUniqueColor());

		ImGui::Begin("Entity Inspector");
		{

			bool changed = false;
			if (m_selectedEntity != INVALID_ID)
				for (auto& ent : m_entities)
					if (ent->GetID() == m_selectedEntity)
						ent->RenderUI(changed, m_resourceManager);
			if (changed)
				for (auto& pipeline : m_pipelines)
					pipeline->EntitiesUnsorted(); // TODO: Only opdate pipeline related to entity

			ImGui::End();
		}
	}

	{
		ZoneScopedNC("Resources", RandomUniqueColor());

		ImGui::Begin("Resources");
		{

			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyleColorVec4(ImGuiCol_FrameBg));

			// Meshes
			{
				ZoneScopedNC("Meshes", RandomUniqueColor());

				ImGui::SeparatorText("Meshes");

				if (ImGui::BeginChild("Meshes", ImVec2(-FLT_MIN, ImGui::GetTextLineHeightWithSpacing() * 8), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeY))
					for (const auto& mesh : m_resourceManager.GetMeshes())
						ImGui::Text("%s", mesh.first.c_str());
				ImGui::EndChild();

				if (ImGui::Button("Add Mesh"))
					ImGui::OpenPopup("Add Mesh");
				
				if (ImGui::BeginPopupModal("Add Mesh", NULL, ImGuiWindowFlags_AlwaysAutoResize))
				{
					static char fileName[32] = ""; 
					ImGui::InputText("File Name", fileName, IM_ARRAYSIZE(fileName));

					static char name[32] = ""; 
					ImGui::InputText("Mesh Name", name, IM_ARRAYSIZE(name));

					if (ImGui::Button("Done"))
					{
						ID id = m_resourceManager.AddMesh("res/ObjectFiles/VentilationRoom/", std::string(fileName), name);
						if (id != INVALID_ID)
							ImGui::CloseCurrentPopup();
						else
							ImGui::OpenPopup("Error");
					}

					ImGui::SameLine();

					if (ImGui::Button("Close"))
						ImGui::CloseCurrentPopup();

					UpdateError();
			
					ImGui::EndPopup();
				}
			}

			// Textures
			{
				ZoneScopedNC("Textures", RandomUniqueColor());

				ImGui::SeparatorText("Textures");

				if (ImGui::BeginChild("Textures", ImVec2(-FLT_MIN, ImGui::GetTextLineHeightWithSpacing() * 8), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeY))
					for (const auto& tex : m_resourceManager.GetTextures())
						ImGui::Text("%s", tex.first.c_str());
				ImGui::EndChild();
			}

			// Shaders
			{
				ZoneScopedNC("Shaders", RandomUniqueColor());

				ImGui::SeparatorText("Shaders");

				if (ImGui::BeginChild("Shaders", ImVec2(-FLT_MIN, ImGui::GetTextLineHeightWithSpacing() * 8), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeY))
					for (const auto& shader : m_resourceManager.GetShaderIDs())
						ImGui::Text("%s", shader.first.c_str());
				ImGui::EndChild();

				if (ImGui::Button("Add Shader"))
					ImGui::OpenPopup("Add Shader");
				
				if (ImGui::BeginPopupModal("Add Shader", NULL, ImGuiWindowFlags_AlwaysAutoResize))
				{
					static char fileName[32] = ""; 
					ImGui::InputText("File Name", fileName, IM_ARRAYSIZE(fileName));

					static char name[32] = ""; 
					ImGui::InputText("Shader Name", name, IM_ARRAYSIZE(name));

					static ShaderType shaderType = ShaderType::VERTEX_SHADER;
					static const char* shaderTypeNames[] =
					{
						"Vertex",
						"Hull",
						"Domain",
						"Geometry",
						"Pixel",
						"Compute"
					};

					int current = static_cast<int>(shaderType);
					if (ImGui::Combo("Shader Type", &current, shaderTypeNames, IM_ARRAYSIZE(shaderTypeNames)))
						shaderType = static_cast<ShaderType>(current);

					if (ImGui::Button("Done"))
					{
						ID id = m_resourceManager.AddShader(std::string(fileName), name, shaderType);
						if (id != INVALID_ID)
							ImGui::CloseCurrentPopup();
						else
							ImGui::OpenPopup("Error");
					}

					ImGui::SameLine();

					if (ImGui::Button("Close"))
						ImGui::CloseCurrentPopup();

					UpdateError();
			
					ImGui::EndPopup();
				}
			}

			ImGui::PopStyleColor();

			ImGui::End();
		}
	}

	InternalRenderUI();

	m_meshInspector.RenderUI();
}

Pipeline* Scene::CreatePipeline()
{
    Pipeline* pipeline = new Pipeline();
    m_pipelines.push_back(pipeline);
    return pipeline;
}

void Scene::OpenMeshInspector()
{
	m_meshInspector.SetActive();
}

bool Scene::PhysicsEnabled() const
{
	return m_physicsEnabled;
}

void Scene::SetPhysicsEnabled(bool enabled)
{
	m_physicsEnabled = enabled;
}

Pipeline* Scene::GetPipeline(unsigned int index)
{
	if (index < m_pipelines.size()) [[likely]]
		return m_pipelines[index];
	return nullptr;
}

const CameraD3D11* Scene::GetActiveCamera() const
{
	return g_cameraManager.GetCamera();
}

Entity* Scene::CreateEntity()
{
    Entity* ent = new Entity();
    m_entities.push_back(ent);
    return ent;
}

void Scene::RemoveEntity(ID entID)
{
	Entity* ent = *std::find_if(m_entities.begin(), m_entities.end(), [&entID](Entity* ent) { return ent->GetID() == entID; });
	RemoveEntity(ent);
}

void Scene::RemoveEntity(Entity* entity)
{
	m_entities.erase(std::remove(m_entities.begin(), m_entities.end(), entity), m_entities.end());
	for (auto pipeline : m_pipelines)
		if (pipeline->HasEntity(entity))
			pipeline->RemoveEntity(entity);
	delete entity;
}

void Scene::GetEntities(std::vector<Entity*>& entities) const
{
	entities.reserve(m_entities.size());
	for (auto& ent : m_entities)
		entities.push_back(ent);
}

void Scene::AddCamera()
{
	ID id = g_cameraManager.AddCamera(g_cameraManager.GenerateDefaultCamera());
	m_cameras.push_back(id);
}
