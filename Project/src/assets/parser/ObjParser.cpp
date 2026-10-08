#include "stdafx.h"

#include "ObjParser.h"

ObjParser::ObjParser(std::string folderPath, std::string fileName, bool blenderImport):
	m_folderPath(folderPath)
{
	ZoneScopedC(RandomUniqueColor());

	FileHandler(m_folderPath + fileName);

	AddObject();

	if (blenderImport)
		Flip();
}

MeshData ObjParser::GetMeshData()
{
	ZoneScopedC(RandomUniqueColor());

	// Load Mesh Data
	MeshData meshData;

	meshData.vertexInfo.nrOfVerticesInBuffer = m_objectsData[0].vertices.size();
	meshData.vertexInfo.sizeOfVertex = sizeof(ShaderVertexUV);
	meshData.vertexInfo.vertexData = m_objectsData[0].vertices.data();

	meshData.indexInfo.nrOfIndicesInBuffer = m_objectsData[0].indices.size();
	meshData.indexInfo.indexData = m_objectsData[0].indices.data();

	// Load SubMesh Data
	MeshData::SubMeshInfo subMeshInfo;
	subMeshInfo.material = m_objectsData[0].material;
	subMeshInfo.nrOfIndicesInSubMesh = m_objectsData[0].indices.size();
	subMeshInfo.startIndexValue = 0;

	meshData.subMeshInfo.push_back(subMeshInfo);
	meshData.BBMin = GetBoundingBoxMin();
	meshData.BBMax = GetBoundingBoxMax();

	return meshData;
}

std::vector<Vertex> ObjParser::GetVertecies()
{
	return m_vertecies;
}

std::vector<VertexTexture> ObjParser::GetVertexTextures()
{
	return m_vertexTextures;
}

std::vector<VertexNormal> ObjParser::GetVertexNormals()
{
	return m_vertexNormals;
}

std::vector<Face> ObjParser::GetFaces()
{
	return m_faces;
}

void ObjParser::AddObject()
{
	ZoneScopedC(RandomUniqueColor());

	std::vector<ShaderVertexUV> vertices;
	std::vector<uint32_t> indices;

	vertices.reserve(m_vertecies.size());
	indices.reserve(m_faces.size() * 3);

	std::vector<Vertex> verts(m_vertecies.begin(), m_vertecies.end());
	std::vector<VertexNormal> vertexNormals(m_vertexNormals.begin(), m_vertexNormals.end());
	std::vector<VertexTexture> vertexTextures(m_vertexTextures.begin(), m_vertexTextures.end());
	std::vector<Face> faces(m_faces.begin(), m_faces.end());

	std::unordered_map<ShaderVertexUV, uint32_t, ShaderVertexUV> vertexMap;
	vertexMap.reserve(m_vertecies.size());

	for (const auto& face : faces)
	{
		if (face.nSets == 3)
		{
			for (int j = 0; j < face.nSets; j++)
			{
				Vertex v = verts[face.iv[j]];
				VertexNormal vn = (face.ivn.size() > 0) ? vertexNormals[face.ivn[j]] : VertexNormal();
				VertexTexture vt = (face.ivt.size() > 0) ? vertexTextures[face.ivt[j]] : VertexTexture();

				ShaderVertexUV tempVertex = { {v.x, v.y, v.z}, {vn.x, vn.y, vn.z}, {vt.u, vt.v} };

				auto it = vertexMap.find(tempVertex);
				uint32_t vertexIndex;
				if (it == vertexMap.end())
				{
					vertexIndex = static_cast<uint32_t>(vertices.size());
					vertices.push_back(tempVertex);
					vertexMap[tempVertex] = vertexIndex;
				}
				else
				{
					vertexIndex = it->second;
				}

				indices.push_back(vertexIndex);
			}
		}
		else if (face.nSets == 4)
		{
			for (int j = 0; j < 3; j++)
			{
				Vertex v = verts[face.iv[j]];
				VertexNormal vn = (face.ivn.size() > 0) ? vertexNormals[face.ivn[j]] : VertexNormal();
				VertexTexture vt = (face.ivt.size() > 0) ? vertexTextures[face.ivt[j]] : VertexTexture();

				ShaderVertexUV tempVertex = { {v.x, v.y, v.z}, {vn.x, vn.y, vn.z}, {vt.u, vt.v} };

				auto it = vertexMap.find(tempVertex);
				uint32_t vertexIndex;
				if (it == vertexMap.end())
				{
					vertexIndex = static_cast<uint32_t>(vertices.size());
					vertices.push_back(tempVertex);
					vertexMap[tempVertex] = vertexIndex;
				}
				else
				{
					vertexIndex = it->second;
				}

				indices.push_back(vertexIndex);
			}

			int arr[3] = { 0, 2, 3 };
			for (int i = 0; i < 3; i++)
			{
				int j = arr[i];
				Vertex v = verts[face.iv[j]];
				VertexNormal vn = (face.ivn.size() > 0) ? vertexNormals[face.ivn[j]] : VertexNormal();
				VertexTexture vt = (face.ivt.size() > 0) ? vertexTextures[face.ivt[j]] : VertexTexture();

				ShaderVertexUV tempVertex = { {v.x, v.y, v.z}, {vn.x, vn.y, vn.z}, {vt.u, vt.v} };

				auto it = vertexMap.find(tempVertex);
				uint32_t vertexIndex;
				if (it == vertexMap.end())
				{
					vertexIndex = static_cast<uint32_t>(vertices.size());
					vertices.push_back(tempVertex);
					vertexMap[tempVertex] = vertexIndex;
				}
				else
				{
					vertexIndex = it->second;
				}

				indices.push_back(vertexIndex);
			}

		}
	}

	Material material = m_materials[m_currentMaterial];
	m_objectsData.push_back({ vertices, indices, material });
}

void ObjParser::Flip()
{
	// Flip Coords
	for (auto& vertex : m_objectsData[0].vertices)
	{
		vertex.position.x *= -1.0f;
		vertex.normal.x *= -1.0f;
		vertex.uv.y = 1 - vertex.uv.y;
	}

	// Update winding order
	for (int i = 0; i < m_objectsData[0].indices.size(); i += 3)
		std::swap(m_objectsData[0].indices[i + 1], m_objectsData[0].indices[i + 2]);
}

void ObjParser::FileHandler(std::string filePath)
{
	ZoneScopedC(RandomUniqueColor());

	// Open file
	std::ifstream file(filePath);

	// Check if file exists
	if (!file.good())
	{
		std::cout << "File: " << filePath << " not found!" << std::endl;
		return;
	}

	std::string line;

	// Iterate over file lines
	while (std::getline(file, line))
		LineParser(line);

	// Close file
	file.close();
}

Vertex ObjParser::ParseVertex(std::string line)
{
	float xPos, yPos, zPos, wPos;
	float rCol, gCol, bCol;

	std::string s;
	std::stringstream ss(line);

	int index = 0;
	while (std::getline(ss, s, ' '))
	{
		float value;
		if (index > 0)
			value = std::stof(s);

		if (index == 1)
			xPos = value;
		else if (index == 2)
			yPos = value;
		else if (index == 3)
			zPos = value;
		else if (index == 4)
			wPos = rCol = value;
		else if (index == 5)
			gCol = value;
		else if (index == 6)
			bCol = value;

		index++;
	}

	if (index == 4)
		return Vertex(xPos, yPos, zPos);
	else if (index == 5)
		return Vertex(xPos, yPos, zPos, wPos);
	else if (index == 7)
		return Vertex(xPos, yPos, zPos, rCol, gCol, bCol);

	return Vertex();
}

VertexTexture ObjParser::ParseVertexTexture(std::string line)
{
	float uPos, vPos, wPos;

	std::string s;
	std::stringstream ss(line);

	int index = 0;
	while (std::getline(ss, s, ' '))
	{
		float value;
		if (index > 0)
			value = std::stof(s);

		if (index == 1)
			uPos = value;
		else if (index == 2)
			vPos = value;
		else if (index == 3)
			wPos = value;

		index++;
	}

	if (index == 2)
		return VertexTexture(uPos);
	else if (index == 3)
		return VertexTexture(uPos, vPos);
	else if (index == 4)
		return VertexTexture(uPos, vPos, wPos);

	return VertexTexture();
}

VertexNormal ObjParser::ParseVertexNormal(std::string line)
{
	float xPos, yPos, zPos;

	std::string s;
	std::stringstream ss(line);

	int index = 0;
	while (std::getline(ss, s, ' '))
	{
		float value;
		if (index > 0)
			value = std::stof(s);

		if (index == 1)
			xPos = value;
		else if (index == 2)
			yPos = value;
		else if (index == 3)
			zPos = value;

		index++;
	}

	if (index == 4)
		return VertexNormal(xPos, yPos, zPos);

	return VertexNormal();
}

Face ObjParser::ParseFace(std::string line)
{
	std::vector<uint32_t> vertexIndex, vertexTextureIndex, vertexNormalIndex;

	std::string s;
	std::stringstream ss(line);

	int setIndex = 0;
	int index = 0;
	while (std::getline(ss, s, ' '))
	{
		if (index > 0)
		{
			setIndex = 0;
			std::string sSet;
			std::stringstream ssSet(s);

			while (std::getline(ssSet, sSet, '/'))
			{
				int value = static_cast<uint32_t>(std::stoul(sSet));
				if (setIndex == 0)
					vertexIndex.push_back(value);
				else if (setIndex == 1)
					vertexTextureIndex.push_back(value);
				else if (setIndex == 2)
					vertexNormalIndex.push_back(value);
 				setIndex++;
			}
		}

		index++;
	}

	if (index == vertexIndex.size()+1)
		return Face(vertexIndex, vertexTextureIndex, vertexNormalIndex, vertexIndex.size());

	return Face();
}

float ObjParser::ParseKd(std::string line)
{
	float val1 = 0, val2 = 0, val3 = 0;

	std::string s;
	std::stringstream ss(line);

	int index = 0;
	while (std::getline(ss, s, ' '))
	{
		float value;
		if (index > 0)
			value = std::stof(s);

		if (index == 1)
			val1 = value;
		else if (index == 2)
			val2 = value;
		else if (index == 3)
			val3 = value;

		index++;
	}

	return val1;
}

float ObjParser::ParseKa(std::string line)
{
	float val1 = 0, val2 = 0, val3 = 0;

	std::string s;
	std::stringstream ss(line);

	int index = 0;
	while (std::getline(ss, s, ' '))
	{
		float value;
		if (index > 0)
			value = std::stof(s);

		if (index == 1)
			val1 = value;
		else if (index == 2)
			val2 = value;
		else if (index == 3)
			val3 = value;

		index++;
	}

	return val1;
}

float ObjParser::ParseKs(std::string line)
{
	float val1 = 0, val2 = 0, val3 = 0;

	std::string s;
	std::stringstream ss(line);

	int index = 0;
	while (std::getline(ss, s, ' '))
	{
		float value;
		if (index > 0)
			value = std::stof(s);

		if (index == 1)
			val1 = value;
		else if (index == 2)
			val2 = value;
		else if (index == 3)
			val3 = value;

		index++;
	}

	return val1;
}

float ObjParser::ParseNs(std::string line)
{
	float val = 0;

	std::string s;
	std::stringstream ss(line);

	int index = 0;
	while (std::getline(ss, s, ' '))
	{
		float value;
		if (index > 0)
			value = std::stof(s);

		if (index == 1)
			val = value;

		index++;
	}

	return val;
}

std::string ObjParser::ParseMtl(std::string line)
{
	return line.erase(0, 7);
}

LineType ObjParser::LineCategorizer(std::string line)
{
	if (line.length() < 2)
		return LineType::NONE;

	if (line.substr(0, 6) == "mtllib")
		return LineType::MTLFILE;
	else if (line.substr(0, 6) == "usemtl")
		return LineType::SETMTL;
	else if (line.substr(0, 6) == "newmtl")
		return LineType::NEWMTL;
	else if (line.substr(0, 2) == "Kd")
		return LineType::KDMTL;
	else if (line.substr(0, 2) == "Ka")
		return LineType::KAMTL;
	else if (line.substr(0, 2) == "Ks")
		return LineType::KSMTL;
	else if (line.substr(0, 2) == "Ns")
		return LineType::NSMTL;
	else if (line.substr(0, 2) == "vt")
		return LineType::VERTEXTEXTURE;
	else if (line.substr(0, 2) == "vn")
		return LineType::VERTEXNORMAL;
	else if (line[0] == 'v')
		return LineType::VERTEX;
	else if (line[0] == 'f')
		return LineType::FACE;
	else
		return LineType::NONE;
}

void ObjParser::LineParser(std::string line)
{
	LineType type = LineCategorizer(line);

	switch (type)
	{
	case VERTEX:
		m_vertecies.push_back(ParseVertex(line));
		break;
	case VERTEXTEXTURE:
		m_vertexTextures.push_back(ParseVertexTexture(line));
		break;
	case VERTEXNORMAL:
		m_vertexNormals.push_back(ParseVertexNormal(line));
		break;
	case FACE:
		m_faces.push_back(ParseFace(line));
		break;
	case MTLFILE:
		FileHandler(m_folderPath+ParseMtl(line));
		break;
	case SETMTL:
		m_currentMaterial = line.erase(0, 7);
		break;
	case NEWMTL:
		m_tempMaterialName = line.erase(0, 7);
		m_materials.insert({ m_tempMaterialName, Material() });
		break;
	case KDMTL:
		m_materials[m_tempMaterialName].diff = ParseKd(line);
		break;
	case KAMTL:
		m_materials[m_tempMaterialName].amb = ParseKa(line);
		break;
	case KSMTL:
		m_materials[m_tempMaterialName].spec = ParseKs(line);
		break;
	case NSMTL:
		m_materials[m_tempMaterialName].shine = ParseNs(line);
	case NONE:
		break;
	default:
		break;
	}
}

uint32_t ObjParser::FindVertex(ShaderVertexUV vertex, const std::vector<ShaderVertexUV>& vertices)
{
	for (int i = 0; i < vertices.size(); i++)
		if (vertices[i] == vertex)
			return i;
	return vertices.size();
}

DX::XMFLOAT3 ObjParser::GetBoundingBoxMax()
{
	if (m_vertecies.size() == 0)
		return DX::XMFLOAT3(0, 0, 0);

	DX::XMFLOAT3 maxPos(m_vertecies[0].x, m_vertecies[0].y, m_vertecies[0].z);
	for (auto& vertex : m_vertecies)
		maxPos = { maxPos.x > vertex.x ? maxPos.x : vertex.x,
				   maxPos.y > vertex.y ? maxPos.y : vertex.y,
				   maxPos.z > vertex.z ? maxPos.z : vertex.z };

	return maxPos;
}

DX::XMFLOAT3 ObjParser::GetBoundingBoxMin()
{
	if (m_vertecies.size() == 0)
		return DX::XMFLOAT3(0, 0, 0);

	DX::XMFLOAT3 minPos(m_vertecies[0].x, m_vertecies[0].y, m_vertecies[0].z);
	for (auto& vertex : m_vertecies)
		minPos = { minPos.x < vertex.x ? minPos.x : vertex.x,
				   minPos.y < vertex.y ? minPos.y : vertex.y,
				   minPos.z < vertex.z ? minPos.z : vertex.z };

	return minPos;
}
