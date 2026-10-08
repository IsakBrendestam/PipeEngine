#include "stdafx.h"

#include "StructuredBufferD3D11.h"

#include "core/rendering/d3d11/DeviceManager.h"

StructuredBufferD3D11::StructuredBufferD3D11(UINT sizeOfElement, size_t nrOfElementsInBuffer, size_t maxElements, void* bufferData, bool dynamic)
{
	Initialize(sizeOfElement, nrOfElementsInBuffer, maxElements, bufferData, dynamic);
}

StructuredBufferD3D11::~StructuredBufferD3D11()
{
	ReleaseCOM(m_buffer);
	ReleaseCOM(m_srv);
	ReleaseCOM(m_uav);
}

void StructuredBufferD3D11::Initialize(UINT sizeOfElement, size_t nrOfElementsInBuffer, size_t maxElements, void* bufferData, bool dynamic)
{
	m_elementSize = sizeOfElement;
	m_elementCount = nrOfElementsInBuffer;
	m_maxElementCount = maxElements;

	// Creating buffer desc
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = m_elementSize * m_maxElementCount;
	desc.Usage = dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	desc.BindFlags =  dynamic ? D3D11_BIND_SHADER_RESOURCE : D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	desc.CPUAccessFlags = dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = m_elementSize;

	if (bufferData != nullptr)
	{
		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = bufferData;
		data.SysMemPitch = data.SysMemSlicePitch = 0;
		HR(DeviceManager::Device()->CreateBuffer(&desc, &data, &m_buffer));
	}
	else
		HR(DeviceManager::Device()->CreateBuffer(&desc, nullptr, &m_buffer));

	// Creating SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = m_maxElementCount;

	HR(DeviceManager::Device()->CreateShaderResourceView(m_buffer, &srvDesc, &m_srv));

	if (!dynamic)
	{
		// Creating UAV
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = m_maxElementCount;
		uavDesc.Buffer.Flags = 0;

		HR(DeviceManager::Device()->CreateUnorderedAccessView(m_buffer, &uavDesc, &m_uav));
	}
}


void StructuredBufferD3D11::UpdateBuffer(void* data)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	DeviceManager::Context()->Map(m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	memcpy(mappedResource.pData, data, m_elementSize * m_elementCount);

	DeviceManager::Context()->Unmap(m_buffer, 0);
}

UINT StructuredBufferD3D11::GetElementSize() const
{
	return m_elementSize;
}

size_t StructuredBufferD3D11::GetElementCount() const
{
	return m_elementCount;
}

void StructuredBufferD3D11::ChangeElementCount(size_t amount)
{
	m_elementCount += amount;

	// Clamp value
	if (m_elementCount > m_maxElementCount)
		m_elementCount = m_maxElementCount;

	if (m_elementCount < 0)
		m_elementCount = 0;
}

ID3D11ShaderResourceView* StructuredBufferD3D11::GetSRV() const
{
	return m_srv;
}

ID3D11UnorderedAccessView* StructuredBufferD3D11::GetUAV() const
{
	return m_uav;
}
