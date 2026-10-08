#include "stdafx.h"

#include "InputLayoutD3D11.h"

#include <d3d11shader.h>
#include <d3dcompiler.h>

#include "core/rendering/d3d11/DeviceManager.h"

InputLayoutD3D11::~InputLayoutD3D11()
{
	ReleaseCOM(m_inputLayout);
}

void InputLayoutD3D11::CreateFromShader(const ShaderD3D11& shader)
{
	/*
	ID3D11ShaderReflection* reflection = nullptr;

	HR(D3DReflect(shader.GetBuffer(), shader.GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&reflection));
	*/
}

void InputLayoutD3D11::AddInputElement(const std::string& semanticName, DXGI_FORMAT format)
{
	m_semanticNames.push_back(semanticName);

	D3D11_INPUT_ELEMENT_DESC desc;
	desc.SemanticName = "";
	desc.SemanticIndex = 0;
	desc.Format = format;
	desc.InputSlot = 0;
	desc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	desc.InstanceDataStepRate = 0;
	m_elements.push_back(desc);
}

void InputLayoutD3D11::FinalizeInputLayout(const void* vsDataPtr, size_t vsDataSize)
{
	D3D11_INPUT_ELEMENT_DESC* data = m_elements.data();
	for (int i = 0; i < m_elements.size(); i++)
		(data+i)->SemanticName = m_semanticNames[i].c_str();

	HR(DeviceManager::Device()->CreateInputLayout(data, m_elements.size(), vsDataPtr, vsDataSize, &m_inputLayout));
}

ID3D11InputLayout* InputLayoutD3D11::GetInputLayout() const
{
	return m_inputLayout;
}
