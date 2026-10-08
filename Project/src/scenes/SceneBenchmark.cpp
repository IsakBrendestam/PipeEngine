#include "stdafx.h"

#include "SceneBenchmark.h"
#include "debug/HardwareData.h"

#include <iostream>
#include <ostream>
#include <string>

struct EffectsControll
{
	UINT effectFlag;
	UINT temp[3]; //Padding
} effectsControll;

struct SettingsCRT
{
	float scanlineSettings[3];  // Strength, frequency, speed
	float distortion;          
	UINT  windowSize[2];
	float vignette;
	float shiftRGB;
	double time;
	float temp; //Padding
} settingsCRT;

struct SettingsBenDay
{
	float resolution[2];
	float dotSpacing;
	float dotScale;
	float angles[3];
	float temp; //Padding
} settingsBenDay2;

SceneBenchmark::~SceneBenchmark()
{
	ZoneScopedC(RandomUniqueColor());
	ReleaseCOM(m_pipelineQuery);
}

void SceneBenchmark::Load()
{
	ZoneScopedC(RandomUniqueColor());

	effectsControll.effectFlag = (1 << 0);

	// Settins CRT
	settingsCRT.scanlineSettings[0] = 0.3f;
	settingsCRT.scanlineSettings[1] = 400.0f;
	settingsCRT.scanlineSettings[2] = 1.8f;
	settingsCRT.windowSize[0] = g_engineSettings.renderResolution[0];
	settingsCRT.windowSize[1] = g_engineSettings.renderResolution[1];
	settingsCRT.distortion = 0.03f;
	settingsCRT.vignette = 0.06f;
	settingsCRT.shiftRGB = 0.003f;
	settingsCRT.time = 0.0f;


	// Settings Ben Day
	settingsBenDay2.resolution[0] = g_engineSettings.renderResolution[0];
	settingsBenDay2.resolution[1] = g_engineSettings.renderResolution[1];

	settingsBenDay2.dotSpacing = 3.66f;
	settingsBenDay2.dotScale = 1.095f;

	settingsBenDay2.angles[0] = 0.26f;
	settingsBenDay2.angles[1] = 0.11f;
	settingsBenDay2.angles[2] = 0.16f;


	ID vs = m_resourceManager.AddShader("Default_VS", "Default_VS", ShaderType::VERTEX_SHADER),
		ps = m_resourceManager.AddShader("Default_PS", "Default_PS", ShaderType::PIXEL_SHADER),
		mesh = m_resourceManager.AddMesh("res/ObjectFiles/", "teapot.obj", "Teapot"),
		mesh2 = m_resourceManager.AddMesh("res/ObjectFiles/", "Cube.obj", "Cube"),
		sponza = m_resourceManager.AddMesh("res/ObjectFiles/", "sponza_4K._noDecalsglb.glb", "Sponza"),
		tex = m_resourceManager.AddTexture("res/Textures/Magenta.png", "Grid"),
		tex2 = m_resourceManager.AddTexture("res/Textures/Spaceships/Imperial_Orange.png", "ImperialOrange"),
		samp = m_resourceManager.AddSampler("Sampler", D3D11_TEXTURE_ADDRESS_WRAP);

	m_cs1 = m_resourceManager.AddShader("post_effects/PostEffect_CS", "PostEffects_CS", ShaderType::COMPUTE_SHADER);
	m_cs2 = m_resourceManager.AddShader("post_effects/PostEffect_CS_S", "PostEffects_CS_S", ShaderType::COMPUTE_SHADER);

	Entity* ent1 = CreateEntity();
	{
		ent1->SetMesh(mesh);
		ent1->SetSampler(samp);
		ent1->SetTransform(Transform({ -3, 4, -4 }, { 0, 0, 0 }, { 0.1f, 0.1f, 0.1f }));
		ent1->SetMaterial(MaterialMaps(tex));
	}

	Entity* ent2 = CreateEntity();
	{
		ent2->SetMesh(mesh2); 
		ent2->SetSampler(samp);
		ent2->SetTransform(Transform({ 5, 4, 4 }, { 0, 0, 0 }, { 1, 1, 1 }));
		ent2->SetMaterial(MaterialMaps(tex));
	}

	Entity* entSponza = CreateEntity();
	{
		entSponza->SetMesh(sponza); 
		entSponza->SetSampler(samp);
		entSponza->SetTransform(Transform({ 0, 0, 0 }, { 0, 0, 0 }, { 0.01, 0.01, 0.01 }));
	}

	PointLight light = PointLight({ 4, 9, -14 }, { 1, 1, 1 }, 150, 1, 8);

	// Default pipeline
	{
		Pipeline* pipeline = CreatePipeline();

		pipeline->SetShader(vs, ShaderType::VERTEX_SHADER);
		std::pair<const char*, DXGI_FORMAT> layout[] = {
			std::make_pair("POSITION", DXGI_FORMAT_R32G32B32_FLOAT),
			std::make_pair("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT),
			std::make_pair("UV", DXGI_FORMAT_R32G32_FLOAT),
			std::make_pair("TANGENT", DXGI_FORMAT_R32G32B32_FLOAT),
			std::make_pair("BITANGENT", DXGI_FORMAT_R32G32B32_FLOAT)
		};
		pipeline->AddInputLayout(layout, 5, m_resourceManager);

		pipeline->SetShader(ps, ShaderType::PIXEL_SHADER);
		DX::XMFLOAT4X4 w, vp;
		DX::XMStoreFloat4x4(&w, DX::XMMatrixIdentity());
		DX::XMStoreFloat4x4(&vp, DX::XMMatrixIdentity());
		DX::XMFLOAT4X4 matrices[2] = { w, vp };
		pipeline->AddConstantBuffer(0, matrices, 2 * sizeof(DX::XMFLOAT4X4), vs, m_resourceManager);

		pipeline->SetShader(m_cs1, ShaderType::COMPUTE_SHADER);
		pipeline->AddConstantBuffer(0, &effectsControll, sizeof(EffectsControll), m_cs1, m_resourceManager);
		pipeline->AddConstantBuffer(1, &settingsCRT, sizeof(SettingsCRT), m_cs1, m_resourceManager);
		pipeline->AddConstantBuffer(2, &settingsBenDay2, sizeof(SettingsBenDay), m_cs1, m_resourceManager);

		pipeline->SetRasterizer(new RasterizerD3D11());

		pipeline->AddEntity(ent1);
		pipeline->AddEntity(entSponza);
		pipeline->AddEntity(ent2);

		pipeline->Disable();

		m_renderer.AddPipeline(pipeline);

		m_defaultPipeline = pipeline;
	}

	// Specialized pipeline
	{
		vs = m_resourceManager.AddShader("Texture_VS", "Texture_VS_S", ShaderType::VERTEX_SHADER);
		ps = m_resourceManager.AddShader("specializedShaders/Texture_PS_S", "Texture_PS_S", ShaderType::PIXEL_SHADER);

		Pipeline* pipeline = CreatePipeline();

		pipeline->SetShader(vs, ShaderType::VERTEX_SHADER);
		std::pair<const char*, DXGI_FORMAT> layout[] = {
			std::make_pair("POSITION", DXGI_FORMAT_R32G32B32_FLOAT),
			std::make_pair("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT),
			std::make_pair("UV", DXGI_FORMAT_R32G32_FLOAT),
			std::make_pair("TANGENT", DXGI_FORMAT_R32G32B32_FLOAT),
			std::make_pair("BITANGENT", DXGI_FORMAT_R32G32B32_FLOAT)
		};
		pipeline->AddInputLayout(layout, 5, m_resourceManager);

		DX::XMFLOAT4X4 w, vp;
		DX::XMStoreFloat4x4(&w, DX::XMMatrixIdentity());
		DX::XMStoreFloat4x4(&vp, DX::XMMatrixIdentity());
		DX::XMFLOAT4X4 matrices[2] = { w, vp };
		pipeline->AddConstantBuffer(0, matrices, 2 * sizeof(DX::XMFLOAT4X4), vs, m_resourceManager);

		pipeline->SetShader(ps, ShaderType::PIXEL_SHADER);

		pipeline->SetShader(m_cs2, ShaderType::COMPUTE_SHADER);
		pipeline->AddConstantBuffer(1, &settingsCRT, sizeof(SettingsCRT), m_cs2, m_resourceManager);
		pipeline->AddConstantBuffer(2, &settingsBenDay2, sizeof(SettingsBenDay), m_cs2, m_resourceManager);

		pipeline->SetRasterizer(new RasterizerD3D11());

		pipeline->AddEntity(ent1);
		pipeline->AddEntity(ent2);
		pipeline->AddEntity(entSponza);

		pipeline->Disable();

		m_renderer.AddPipeline(pipeline);

		m_specializedPipeline = pipeline;
	}

	HardwareData data;
	CollectHardwareData(data);

	std::cout << data << std::endl;

	D3D11_QUERY_DESC queryDesc = {};
	queryDesc.Query = D3D11_QUERY_PIPELINE_STATISTICS;
	queryDesc.MiscFlags = 0;

	ReleaseCOM(m_pipelineQuery);
	HR(DeviceManager::Device()->CreateQuery(&queryDesc, &m_pipelineQuery));

	m_pipelineData.reserve(m_interval * 4);
}

void SceneBenchmark::Unload()
{
	ReleaseCOM(m_pipelineQuery);
}

void SceneBenchmark::SaveBenchmarkData(const char* fileName)
{
	std::ofstream timeFile;
	timeFile.open(std::format("data/{}.json", fileName).c_str());

	timeFile << "{\n";
	auto& timings = g_time.GetAllAverageScopeTimes();
	for (const auto& [name, time] : timings)
		timeFile << "\"" + name + "\": " << time << ",";
	timeFile << "\n}";


	std::ofstream dataFile;
	dataFile.open(std::format("data/{}_pipeline.json", fileName).c_str());

	dataFile << "{";
	for (size_t i = 0; i < m_pipelineData.size(); i++)
	{
		const auto& data = m_pipelineData[i];

		if (i != 0)
			dataFile << ",";

		dataFile << "\"Frame " << i << "\" : " << " {";
		dataFile << "\"" + VariableName(data.IAVertices)	+ "\":" << data.IAVertices		<< ",";
		dataFile << "\"" + VariableName(data.IAPrimitives)	+ "\":" << data.IAPrimitives	<< ",";
		dataFile << "\"" + VariableName(data.VSInvocations) + "\":" << data.VSInvocations	<< ",";
		dataFile << "\"" + VariableName(data.PSInvocations) + "\":" << data.PSInvocations	<< ",";
		dataFile << "\"" + VariableName(data.CInvocations)	+ "\":" << data.CInvocations	<< ",";
		dataFile << "\"" + VariableName(data.CSInvocations) + "\":" << data.CSInvocations	<< "";
		dataFile << "}";
	}
	dataFile << "}";

	m_pipelineData.clear();
}

void SceneBenchmark::Update()
{
	ZoneScopedC(RandomUniqueColor());

	static bool first = true;
	if (first)
	{
		SaveBenchmarkData(std::format("DataInitial_{}", g_engineSettings.renderResolution[1]).c_str());
		first = false;
		g_time.ResetAverageTimes();
	}

	settingsCRT.time += g_time.DeltaTime<double>() / 1000;
	GetPipeline(0)->UpdateConstantBuffer(1, &settingsCRT, m_cs1, m_resourceManager);
	GetPipeline(1)->UpdateConstantBuffer(1, &settingsCRT, m_cs2, m_resourceManager);

	static unsigned int frame = 0;

	switch (m_stage)
	{
	case WAIT:
		if (frame == m_waitFrames)
		{
			frame = 0;
			if (m_lastRunStage == BenchmarkStage::DONE)
				m_stage = BenchmarkStage::DEFAULT;
			else if (m_lastRunStage == BenchmarkStage::DEFAULT)
				m_stage = BenchmarkStage::SPECIALIZED;
		}
		else if (frame > m_waitFrames)
		{
			frame = 0;
		}
		break;

	case DEFAULT:
		if (DefaultBenchmark(frame))
		{
			m_stage = BenchmarkStage::WAIT;
			m_lastRunStage = BenchmarkStage::DEFAULT;
		}
		CollectLastRenderStats();
		break;

	case SPECIALIZED:
		if (SpecializedBenchmark(frame))
		{
			m_stage = BenchmarkStage::DONE;
			m_lastRunStage = BenchmarkStage::SPECIALIZED;
		}
		CollectLastRenderStats();
		break;

	case DONE:
	default:
		break;
	}

	frame++;
}

void SceneBenchmark::PreRender()
{
	if (m_running)
		DeviceManager::Context()->Begin(m_pipelineQuery);
}

void SceneBenchmark::PostRender()
{
	if (m_running)
		DeviceManager::Context()->End(m_pipelineQuery);
}

void SceneBenchmark::InternalRenderUI()
{
	ZoneScopedC(RandomUniqueColor());

	ImGui::Begin("Settings");

	static int flag = 0;
	int tempFlag = flag;
	if (ImGui::InputInt("Effect", &tempFlag, 1, 1))
		if (tempFlag >= 0 && tempFlag < 3)
		{
			flag = tempFlag;
			effectsControll.effectFlag = (1 << tempFlag);
			m_defaultPipeline->UpdateConstantBuffer(0, &effectsControll, m_cs1, m_resourceManager);
			ShaderD3D11* shader = m_resourceManager.GetShaderMutable(m_cs2);
			shader->SetMacroValue("EFFECT_FLAG", std::format("(1 << {})", tempFlag).c_str());
			shader->Recompile();
		}

	if (ImGui::CollapsingHeader("CRT"))
	{
		if (ImGui::TreeNode("Scanline"))
		{
			ImGui::DragFloat("Strength", &settingsCRT.scanlineSettings[0], 0.001f, 0.0f, 3.0f);
			ImGui::DragFloat("Frequency", &settingsCRT.scanlineSettings[1], 1.0f, 0.0f, 800.0f);
			ImGui::DragFloat("Speed", &settingsCRT.scanlineSettings[2], 0.01f, -100.0f, 100.0f);
			ImGui::TreePop();
		}

		ImGui::DragFloat("Distortion", &settingsCRT.distortion, 0.001f, 0.0f, 0.3f);
		ImGui::DragFloat("Vignette", &settingsCRT.vignette, 0.001f, 0.0f, 0.3f);
		ImGui::DragFloat("Shift RGB", &settingsCRT.shiftRGB, 0.0001f, -0.3f, 0.03f, "%.4f");
	}

	if (ImGui::CollapsingHeader("Ben Day"))
	{
		bool changed = false;
		changed |= ImGui::DragFloat2("Resolution", settingsBenDay2.resolution, 0.1f, 0.0f, 1920);
		changed |= ImGui::DragFloat("Dot Scale", &settingsBenDay2.dotScale, 0.001f, 0.0f, 10);
		changed |= ImGui::DragFloat("Dot Spacing", &settingsBenDay2.dotSpacing, 0.01f, 0.0f, 50);
		changed |= ImGui::DragFloat4("Rotation", settingsBenDay2.angles, 0.01f, 0.0f, 360);

		if (changed)
		{
			GetPipeline(0)->UpdateConstantBuffer(2, &settingsBenDay2, m_cs1, m_resourceManager);
			GetPipeline(1)->UpdateConstantBuffer(2, &settingsBenDay2, m_cs2, m_resourceManager);
		}
	}

	if (ImGui::Button("Restart"))
		Restart();

	ImGui::InputInt("Frame Interval", (int*)&m_interval, 1, 100);

	ImGui::End();
}

void SceneBenchmark::Restart()
{
	m_stage = BenchmarkStage::WAIT;
	m_lastRunStage = BenchmarkStage::DONE;

	Load();
}

bool SceneBenchmark::DefaultBenchmark(unsigned int frame)
{
	if (frame == 1)
	{
		m_running = true;

		m_defaultPipeline->Enable();
		m_specializedPipeline->Disable();

		effectsControll.effectFlag = (1 << 0);
		m_defaultPipeline->UpdateConstantBuffer(0, &effectsControll, m_cs1, m_resourceManager);
	}
	else if (frame == m_interval*1 + 1)
	{
		effectsControll.effectFlag = (1 << 1);
		m_defaultPipeline->UpdateConstantBuffer(0, &effectsControll, m_cs1, m_resourceManager);
	}
	else if (frame == m_interval*2 + 1)
	{
		effectsControll.effectFlag = (1 << 2);
		m_defaultPipeline->UpdateConstantBuffer(0, &effectsControll, m_cs1, m_resourceManager);
	}
	else if (frame == m_interval*3 + 1)
	{
		effectsControll.effectFlag = (1 << 3);
		m_defaultPipeline->UpdateConstantBuffer(0, &effectsControll, m_cs1, m_resourceManager);
	}
	else if (frame == m_interval*4 + 1)
	{
		SaveBenchmarkData(std::format("Data_{}", g_engineSettings.renderResolution[1]).c_str());
		g_time.ResetAverageTimes();

		m_defaultPipeline->Disable();
		m_specializedPipeline->Disable();

		m_running = false;
	}

	return frame == m_interval*4 + 1;
}

bool SceneBenchmark::SpecializedBenchmark(unsigned int frame)
{
	if (frame == 1)
	{
		m_running = true;
		m_defaultPipeline->Disable();
		m_specializedPipeline->Enable();

		ShaderD3D11* shader = m_resourceManager.GetShaderMutable(m_cs2);
		shader->SetMacroValue("EFFECT_FLAG", "(1 << 0)");
		shader->Recompile();
	}
	else if (frame == m_interval*1 + 1)
	{
		ShaderD3D11* shader = m_resourceManager.GetShaderMutable(m_cs2);
		shader->SetMacroValue("EFFECT_FLAG", "(1 << 1)");
		shader->Recompile();
	}
	else if (frame == m_interval*2 + 1)
	{
		ShaderD3D11* shader = m_resourceManager.GetShaderMutable(m_cs2);
		shader->SetMacroValue("EFFECT_FLAG", "(1 << 2)");
		shader->Recompile();
	}
	else if (frame == m_interval*3 + 1)
	{
		ShaderD3D11* shader = m_resourceManager.GetShaderMutable(m_cs2);
		shader->SetMacroValue("EFFECT_FLAG", "(1 << 3)");
		shader->Recompile();
	}
	else if (frame == m_interval*4 + 1)
	{
		SaveBenchmarkData(std::format("DataS_{}", g_engineSettings.renderResolution[1]).c_str());
		g_time.ResetAverageTimes();
		m_defaultPipeline->Disable();
		m_specializedPipeline->Disable();
		m_running = false;
	}

	return frame == m_interval*4 + 1;
}

void SceneBenchmark::CollectLastRenderStats()
{
	if (!m_running)
		return;

	D3D11_QUERY_DATA_PIPELINE_STATISTICS stats;
	while (DeviceManager::Context()->GetData(m_pipelineQuery, &stats, sizeof(stats), 0) == S_FALSE)
		; // Wait

	m_pipelineData.push_back(stats);

	//DebugLog("vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv");
	//DebugLog(std::format("Vertecies:				{}", stats.IAVertices));
	//DebugLog(std::format("Primitives:				{}", stats.IAPrimitives));
	//DebugLog(std::format("Vertex Invocations:		{}", stats.VSInvocations));
	//DebugLog(std::format("Pixel Invocations:		{}", stats.PSInvocations));
	//DebugLog(std::format("Rasterizer Invocations:	{}", stats.CInvocations));
	//DebugLog(std::format("Compute Invocations:	{}", stats.CSInvocations));
	//DebugLog("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
}
