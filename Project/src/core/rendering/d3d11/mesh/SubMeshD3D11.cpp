#include "stdafx.h"

#include "SubMeshD3D11.h"

#include "core/rendering/d3d11/DeviceManager.h"

SubMeshD3D11::SubMeshD3D11(size_t startIndexValue, size_t nrOfIndicesInSubMesh, Material material)
{
	Initialize(startIndexValue, nrOfIndicesInSubMesh, material);
}

SubMeshD3D11::~SubMeshD3D11()
{
	delete m_materialBuffer;
}

void SubMeshD3D11::Initialize(size_t startIndexValue, size_t nrOfIndicesInSubMesh, Material material)
{
	m_startIndex = startIndexValue;
	m_nrOfIndices = nrOfIndicesInSubMesh;

	Material mat = material;
	m_materialBuffer = new ConstantBufferD3D11(sizeof(Material), false, &mat);
}

void SubMeshD3D11::Draw() const
{
	// Draw Submesh
	TracyD3D11ZoneC(g_tracyContext, "Draw Sub Mesh", RandomUniqueColor());
	InternalScopeGPU(std::format("Draw Sub Mesh: {}", m_nrOfIndices));

	DeviceManager::Context()->DrawIndexed(m_nrOfIndices, m_startIndex, 0);
}

