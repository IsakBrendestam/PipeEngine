#pragma once

#include <d3d11_4.h>
#include "platform/D3D11Helper.h"

class StructuredBufferD3D11
{
private:
	ID3D11Buffer* m_buffer = nullptr;
	ID3D11ShaderResourceView* m_srv = nullptr;
	ID3D11UnorderedAccessView* m_uav = nullptr;
	UINT m_elementSize = 0;
	size_t m_maxElementCount = 0;
	size_t m_elementCount = 0;

public:
	StructuredBufferD3D11() = default;
	StructuredBufferD3D11(UINT sizeOfElement,
		size_t nrOfElementsInBuffer, size_t maxElements, void* bufferData = nullptr, bool dynamic = true);
	~StructuredBufferD3D11();
	StructuredBufferD3D11(const StructuredBufferD3D11& other) = delete;
	StructuredBufferD3D11& operator=(const StructuredBufferD3D11& other) = delete;
	StructuredBufferD3D11(StructuredBufferD3D11&& other) = delete;
	StructuredBufferD3D11 operator=(StructuredBufferD3D11&& other) = delete;

	void Initialize(UINT sizeOfElement,
		size_t nrOfElementsInBuffer, size_t maxElements, void* bufferData = nullptr, bool dynamic = true);

	void UpdateBuffer(void* data);

	UINT GetElementSize() const;
	size_t GetElementCount() const;
	void ChangeElementCount(size_t amount);
	ID3D11ShaderResourceView* GetSRV() const;
	ID3D11UnorderedAccessView* GetUAV() const;
};