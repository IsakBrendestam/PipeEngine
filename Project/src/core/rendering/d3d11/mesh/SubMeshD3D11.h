#pragma once

#include <string>

#include "core/graphics/Material.h"
#include "core/rendering/d3d11/buffers/ConstantBufferD3D11.h"
#include "core/EngineSettings.h"

#include <d3d11_4.h>

class SubMeshD3D11
{
private:
	size_t m_startIndex = 0;
	size_t m_nrOfIndices = 0;
	ConstantBufferD3D11* m_materialBuffer;

public:
	SubMeshD3D11() = default;
	SubMeshD3D11(size_t startIndexValue, size_t nrOfIndicesInSubMesh, Material material);
	~SubMeshD3D11();
	SubMeshD3D11(const SubMeshD3D11& other) = default;
	SubMeshD3D11& operator=(const SubMeshD3D11& other) = default;
	SubMeshD3D11(SubMeshD3D11&& other) = default;
	SubMeshD3D11& operator=(SubMeshD3D11&& other) = default;

	void Initialize(size_t startIndexValue, size_t nrOfIndicesInSubMesh, Material material);

	void Draw() const;
};