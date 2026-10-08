#include "stdafx.h"

#include "MeshD3D11.h"

#include "core/rendering/d3d11/DeviceManager.h"

MeshD3D11::MeshD3D11(MeshData meshInfo)
{
	Initialize(meshInfo);
}

MeshD3D11::~MeshD3D11()
{
	ZoneScopedC(RandomUniqueColor());

	delete m_vertexBuffer;
	delete m_indexBuffer;

	for (auto& subMesh : m_subMeshes)
		delete subMesh;
}

void MeshD3D11::Initialize(MeshData meshInfo)
{
	ZoneScopedC(RandomUniqueColor());

	m_meshData = meshInfo;

	m_vertexBuffer = new VertexBufferD3D11(meshInfo.vertexInfo.sizeOfVertex, meshInfo.vertexInfo.nrOfVerticesInBuffer, meshInfo.vertexInfo.vertexData);

	m_indexBuffer = new IndexBufferD3D11(meshInfo.indexInfo.nrOfIndicesInBuffer, meshInfo.indexInfo.indexData);

	// Add SubMeshes
	for (auto& subMeshInfo : meshInfo.subMeshInfo)
	{
		m_subMeshes.push_back(new SubMeshD3D11(subMeshInfo.startIndexValue, subMeshInfo.nrOfIndicesInSubMesh, subMeshInfo.material));
	}
}

void MeshD3D11::BindMeshBuffers() const
{
	ZoneScopedC(RandomUniqueColor());

	m_vertexBuffer->Bind(0);
	m_indexBuffer->Bind();
}

void MeshD3D11::DrawMesh() const
{
	ZoneScopedC(RandomUniqueColor());

	for (int i = 0; i < m_subMeshes.size(); i++)
		DrawSubMesh(i);
}

void MeshD3D11::DrawSubMesh(size_t subMeshIndex) const
{
	ZoneScopedC(RandomUniqueColor());

	m_subMeshes[subMeshIndex]->Draw();
}

DX::XMFLOAT3 MeshD3D11::GetBoundingBoxMin() const
{
	return m_meshData.BBMin;
}

DX::XMFLOAT3 MeshD3D11::GetBoundingBoxMax() const
{
	return m_meshData.BBMax;
}

size_t MeshD3D11::GetNrOfSubMeshes() const
{
	return m_subMeshes.size();
}

MeshData MeshD3D11::GetMeshData() const
{
	return m_meshData;
}

std::vector<MeshData::SubMeshInfo> MeshD3D11::GetSubMeshInfo() const
{
	return m_meshData.subMeshInfo;
}

