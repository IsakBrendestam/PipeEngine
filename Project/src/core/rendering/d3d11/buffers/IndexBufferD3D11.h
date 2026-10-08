#pragma once

#include <cstdint>

#include <d3d11_4.h>
#include "platform/D3D11Helper.h"

class IndexBufferD3D11
{
private:
	ID3D11Buffer* m_buffer = nullptr;
	size_t m_nrOfIndices = 0;

public:
	IndexBufferD3D11() = default;
	IndexBufferD3D11(size_t nrOfIndicesInBuffer, uint32_t* indexData);
	~IndexBufferD3D11();
	IndexBufferD3D11(const IndexBufferD3D11& other) = delete;
	IndexBufferD3D11& operator=(const IndexBufferD3D11& other) = delete;
	IndexBufferD3D11(IndexBufferD3D11&& other) = delete;
	IndexBufferD3D11& operator=(IndexBufferD3D11&& other) = delete;

	void Initialize(size_t nrOfIndicesInBuffer, uint32_t* indexData);

	void Bind() const;

	size_t GetNrOfIndices() const;
	ID3D11Buffer* GetBuffer() const;
};