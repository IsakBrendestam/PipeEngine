#include "stdafx.h"

#include "VertexBufferD3D11.h"

#include "core/rendering/d3d11/DeviceManager.h"

VertexBufferD3D11::VertexBufferD3D11(UINT sizeOfVertex, UINT nrOfVerticesInBuffer, void* vertexData)
{
	Initialize(sizeOfVertex, nrOfVerticesInBuffer, vertexData);
}

VertexBufferD3D11::~VertexBufferD3D11()
{
	ReleaseCOM(m_buffer);
}

void VertexBufferD3D11::Initialize(UINT sizeOfVertex, UINT nrOfVerticesInBuffer, void* vertexData)
{
	m_vertexSize = sizeOfVertex;
	m_nrOfVertices = nrOfVerticesInBuffer;

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = m_vertexSize*m_nrOfVertices;
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = vertexData;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	// Create Buffer Resource
	HR(DeviceManager::Device()->CreateBuffer(&bufferDesc, &data, &m_buffer));
}

void VertexBufferD3D11::Bind(UINT slot) const
{
	UINT stride = m_vertexSize;
	UINT offset = 0;
	DeviceManager::Context()->IASetVertexBuffers(slot, 1, &m_buffer, &stride, &offset);
}

UINT VertexBufferD3D11::GetNrOfVertices() const
{
	return m_nrOfVertices;
}

UINT VertexBufferD3D11::GetVertexSize() const
{
	return m_vertexSize;
}

ID3D11Buffer* VertexBufferD3D11::GetBuffer() const
{
	return m_buffer;
}
