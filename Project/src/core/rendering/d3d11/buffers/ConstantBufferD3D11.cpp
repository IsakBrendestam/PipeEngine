#include "stdafx.h"

#include "ConstantBufferD3D11.h"

#include "core/rendering/d3d11/DeviceManager.h"

ConstantBufferD3D11::ConstantBufferD3D11(size_t byteSize, bool dynamic, void* initialData)
{
	Initialize(byteSize, dynamic, initialData);
}

ConstantBufferD3D11::~ConstantBufferD3D11()
{
	ReleaseCOM(m_buffer);
}

void ConstantBufferD3D11::Initialize(size_t byteSize, bool dynamic, void* initialData)
{
	m_bufferSize = byteSize;

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = m_bufferSize;
	bufferDesc.Usage = dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = initialData;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	// Creating Buffer Resource
	HR(DeviceManager::Device()->CreateBuffer(&bufferDesc, &data, &m_buffer));
}

size_t ConstantBufferD3D11::GetSize() const
{
	return m_bufferSize;
}

ID3D11Buffer* ConstantBufferD3D11::GetBuffer() const
{
	return m_buffer;
}

void ConstantBufferD3D11::UpdateBuffer(void* data)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	DeviceManager::Context()->Map(m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	memcpy(mappedResource.pData, data, m_bufferSize);

	DeviceManager::Context()->Unmap(m_buffer, 0);
}
