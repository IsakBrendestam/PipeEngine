#pragma once

#include <vector>

#include <DirectXMath.h>
#include <d3d11_4.h>

#include "SubMeshD3D11.h"
#include "core/rendering/d3d11/buffers/VertexBufferD3D11.h"
#include "core/rendering/d3d11/buffers/IndexBufferD3D11.h"

#include "core/graphics/ShaderVertex.h"

namespace DX = DirectX;

struct MeshData
{
	struct VertexInfo
	{
		size_t sizeOfVertex;
		size_t nrOfVerticesInBuffer;
		void* vertexData;
	} vertexInfo;

	struct IndexInfo
	{
		size_t nrOfIndicesInBuffer;
		uint32_t* indexData;
	} indexInfo;

	struct SubMeshInfo
	{
		Material material;
		MaterialMaps materialMaps;
		size_t startIndexValue;
		size_t nrOfIndicesInSubMesh;
	};

	std::vector<SubMeshInfo> subMeshInfo;

	DX::XMFLOAT3 BBMin = {}, BBMax = {};
};

class MeshD3D11
{
public:
	MeshD3D11() = default;
	MeshD3D11(MeshData meshInfo);
	~MeshD3D11();
	MeshD3D11(const MeshD3D11 & other) = delete;
	MeshD3D11& operator=(const MeshD3D11 & other) = delete;
	MeshD3D11(MeshD3D11 && other) = delete;
	MeshD3D11& operator=(MeshD3D11 && other) = delete;

	void Initialize(MeshData meshInfo);

	void BindMeshBuffers() const;
	void DrawMesh() const;
	void DrawSubMesh(size_t subMeshIndex) const;

	DX::XMFLOAT3 GetBoundingBoxMin() const;
	DX::XMFLOAT3 GetBoundingBoxMax() const;

	size_t GetNrOfSubMeshes() const;
	MeshData GetMeshData() const;
	std::vector<MeshData::SubMeshInfo> GetSubMeshInfo() const;

private:
	std::vector<SubMeshD3D11*> m_subMeshes;
	VertexBufferD3D11* m_vertexBuffer;
	IndexBufferD3D11* m_indexBuffer;

	MeshData m_meshData;
};