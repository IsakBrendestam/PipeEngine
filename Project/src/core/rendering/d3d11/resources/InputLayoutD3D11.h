#pragma once

#include <string>
#include <vector>

#include <d3d11_4.h>

#include "platform/D3D11Helper.h"
#include "ShaderD3D11.h"

class InputLayoutD3D11
{
private:
	std::vector<std::string> m_semanticNames; // Needed to store the semantic names of the element descs
	std::vector<D3D11_INPUT_ELEMENT_DESC> m_elements;
	ID3D11InputLayout* m_inputLayout = nullptr;

public:
	InputLayoutD3D11() = default;
	~InputLayoutD3D11();
	InputLayoutD3D11(const InputLayoutD3D11& other) = delete;
	InputLayoutD3D11& operator=(const InputLayoutD3D11& other) = delete;
	InputLayoutD3D11(InputLayoutD3D11&& other) = delete;
	InputLayoutD3D11& operator=(InputLayoutD3D11&& other) = delete;

	void CreateFromShader(const ShaderD3D11& shader);

	void AddInputElement(const std::string& semanticName, DXGI_FORMAT format);
	void FinalizeInputLayout(const void* vsDataPtr, size_t vsDataSize);

	ID3D11InputLayout* GetInputLayout() const;
};