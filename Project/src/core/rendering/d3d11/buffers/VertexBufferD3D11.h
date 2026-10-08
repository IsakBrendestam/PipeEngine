#pragma once

#include <d3d11_4.h>
#include "platform/D3D11Helper.h"

class VertexBufferD3D11
{
private:
	ID3D11Buffer* m_buffer = nullptr;
	UINT m_nrOfVertices = 0;
	UINT m_vertexSize = 0;

public:
	VertexBufferD3D11() = default;
	VertexBufferD3D11(UINT sizeOfVertex, 
		UINT nrOfVerticesInBuffer, void* vertexData);
	~VertexBufferD3D11();
	VertexBufferD3D11(const VertexBufferD3D11& other) = delete;
	VertexBufferD3D11& operator=(const VertexBufferD3D11& other) = delete;
	VertexBufferD3D11(VertexBufferD3D11&& other) = delete;
	VertexBufferD3D11& operator=(VertexBufferD3D11&& other) = delete;

	void Initialize(UINT sizeOfVertex,
		UINT nrOfVerticesInBuffer, void* vertexData);

	void Bind(UINT slot) const;

	UINT GetNrOfVertices() const;
	UINT GetVertexSize() const;

	ID3D11Buffer* GetBuffer() const;
};