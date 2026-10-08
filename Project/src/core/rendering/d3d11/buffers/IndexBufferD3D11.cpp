#include "stdafx.h"

#include "IndexBufferD3D11.h"

#include "core/rendering/d3d11/DeviceManager.h"

IndexBufferD3D11::IndexBufferD3D11(size_t nrOfIndicesInBuffer, uint32_t* indexData)
{
	Initialize(nrOfIndicesInBuffer, indexData);
}

IndexBufferD3D11::~IndexBufferD3D11()
{
	ReleaseCOM(m_buffer);
}

void IndexBufferD3D11::Initialize(size_t nrOfIndicesInBuffer, uint32_t* indexData)
{
	m_nrOfIndices = nrOfIndicesInBuffer;

	D3D11_BUFFER_DESC desc;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.ByteWidth = sizeof(uint32_t) * m_nrOfIndices;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = indexData;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	// Create Buffer Resource
	HR(DeviceManager::Device()->CreateBuffer(&desc, &data, &m_buffer));
}

void IndexBufferD3D11::Bind() const
{
	DeviceManager::Context()->IASetIndexBuffer(m_buffer, DXGI_FORMAT_R32_UINT, 0);
}

size_t IndexBufferD3D11::GetNrOfIndices() const
{
	return m_nrOfIndices;
}

ID3D11Buffer* IndexBufferD3D11::GetBuffer() const
{
	return m_buffer;
}
