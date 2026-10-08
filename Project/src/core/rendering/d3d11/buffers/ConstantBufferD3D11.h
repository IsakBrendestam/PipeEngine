#pragma once

#include "platform/D3D11Helper.h"
#include <d3d11_4.h>

class ConstantBufferD3D11
{
private:
	ID3D11Buffer* m_buffer = nullptr;
	size_t m_bufferSize = 0;

public:
	ConstantBufferD3D11() = default;
	ConstantBufferD3D11(size_t byteSize, bool dynamic = true, void* initialData = nullptr);
	~ConstantBufferD3D11();
	ConstantBufferD3D11(const ConstantBufferD3D11& other) = delete;
	ConstantBufferD3D11& operator=(const ConstantBufferD3D11& other) = delete;
	ConstantBufferD3D11(ConstantBufferD3D11&& other) noexcept = delete; // Move constructor
	ConstantBufferD3D11& operator=(ConstantBufferD3D11&& other) noexcept = delete; // Move assignment operator

	void Initialize(size_t byteSize, bool dynamic = true, void* initialData = nullptr);

	size_t GetSize() const;
	ID3D11Buffer* GetBuffer() const;

	void UpdateBuffer(void* data);
};