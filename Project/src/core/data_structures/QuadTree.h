#pragma once

#include <memory>
#include <iostream>
#include <DirectXCollision.h>
#include <systems/CameraManager.h>

#include "platform/D3D11Helper.h"

namespace DX = DirectX;

#define LEAF_MAX_OBJECTS 8
#define TREE_MAX_DEPTH 10

/*
Quads:
	  L	  R
	+---+---+
  U	| 0 | 1	|
	+---+---+
  L	| 2	| 3	|
	+---+---+
*/

enum Quads
{
	LowerLeft = 0,
	LowerRight = 1,
	UpperLeft = 2,
	UpperRight = 3
};

template<typename T>
class QuadTree
{
private:
	struct Node
	{
		int id;
		int nElements = 0;
		const T* elements[LEAF_MAX_OBJECTS] { nullptr };
		DX::BoundingBox elementBoundingBoxes[LEAF_MAX_OBJECTS];

		DX::BoundingBox nodeBoundingBox;
		std::unique_ptr<Node> children[4];

		bool IsLeaf();
		bool IsFull();
	};

public:
	QuadTree(DX::BoundingBox boundingBox);

	void SaveBoundingBoxes(std::string path);
	void PrintTree();
	void AddElement(const T* elementAddress, DX::BoundingBox boundingBox);
	void AddNode(int id);

	std::vector<const T*> CullFrustum(const DX::BoundingFrustum& frustum);

private:
	void SaveBoundingBoxes(std::ofstream* outStream, std::unique_ptr<Node>& node, int& count);
	void PrintTree(std::unique_ptr<Node>& node, int depth);
	void AddElement(const T* elementAddress, DX::BoundingBox boundingBox, std::unique_ptr<Node>& startNode, int depth);
	void AddNode(int id, std::unique_ptr<Node>& startNode);

	void CullFrustum(const DX::BoundingFrustum& frustum, const std::unique_ptr<Node>& node, std::vector<const T*>& objectsToRender);

private:
	std::unique_ptr<Node> root;
};

template<typename T>
inline bool QuadTree<T>::Node::IsLeaf()
{
	for (auto& child : children)
		if (child)
			return false;
	return true;
}

template<typename T>
inline bool QuadTree<T>::Node::IsFull()
{
	return nElements >= LEAF_MAX_OBJECTS;
}

template<typename T>
inline QuadTree<T>::QuadTree(DX::BoundingBox boundingBox)
{
	root = std::make_unique<Node>();
	root->nodeBoundingBox = boundingBox;
	root->id = 0;
}

template<typename T>
inline void QuadTree<T>::SaveBoundingBoxes(std::string path)
{
	std::ofstream outStream;
	outStream.open(path);
	outStream << "id,x,y,z,type" << std::endl;

	DX::BoundingFrustum frustum;
	frustum.CreateFromMatrix(frustum, CameraManager::GetControllCamera()->GetProjectionMatrix()); // Transpose??
	frustum.Transform(frustum, CameraManager::GetControllCamera()->GetWorldMatrix());

	DX::XMFLOAT3 corners[8];
	frustum.GetCorners(corners);

	for (int i = 0; i < 8; i++)
		outStream << 0 << "," << corners[i].x << "," << corners[i].y << "," << corners[i].z << ",FRUSTUM" << std::endl;

	int count = 1;
	SaveBoundingBoxes(&outStream, root, count);

	outStream.close();
}

template<typename T>
inline void QuadTree<T>::PrintTree()
{
	std::cout << "R" << std::endl;
	PrintTree(root, 1);
}

template<typename T>
inline void QuadTree<T>::AddNode(int id)
{
	AddNode(id, root);
}

template<typename T>
inline std::vector<const T*> QuadTree<T>::CullFrustum(const DX::BoundingFrustum& frustum)
{
	std::vector<const T*> objectsToRender;

	CullFrustum(frustum, root, objectsToRender);

	return objectsToRender;
}

template<typename T>
inline void QuadTree<T>::SaveBoundingBoxes(std::ofstream* outStream, std::unique_ptr<Node>& node, int& count)
{
	DX::XMFLOAT3 corners[8];
	node->nodeBoundingBox.GetCorners(corners);
	for (int i = 0; i < 8; i++)
		*outStream << count << "," << corners[i].x << "," << corners[i].y << "," << corners[i].z << ",NODE" << std::endl;

	for (int i = 0; i < node->nElements; i++)
	{
		count++;
		DX::BoundingBox box = node->elements[i]->GetBoundingBox();
		DX::XMFLOAT3 elementCorners[8];
		box.GetCorners(elementCorners);

		for (int i = 0; i < 8; i++)
			*outStream << count << "," << elementCorners[i].x << "," << elementCorners[i].y << "," << elementCorners[i].z << ",OBJECT" << std::endl;
	}

	count++;
	for (auto& child : node->children)
		if (child)
			SaveBoundingBoxes(outStream, child, count);
}

template<typename T>
inline void QuadTree<T>::PrintTree(std::unique_ptr<Node>& node, int depth)
{
	std::string s(depth, '-');
	std::cout << s << "Node" << std::endl;
	for (int i = 0; i < node->nElements; i++)
		std::cout << s << node->elements[i]->GetID() << std::endl;

	for (auto& child : node->children)
		if (child)
			PrintTree(child, depth+1);
}

template<typename T>
inline void QuadTree<T>::AddNode(int id, std::unique_ptr<Node>& startNode)
{
	if (startNode->children[3])
		AddNode(id, startNode->children[0]);

	for (int i = 0; i < 4; i++)
		if (!startNode->children[i])
		{
			startNode->children[i] = std::make_unique<Node>();
			startNode->children[i]->id = id;
			break;
		}
}

template<typename T>
inline void QuadTree<T>::CullFrustum(const DX::BoundingFrustum& frustum, const std::unique_ptr<Node>& node, std::vector<const T*>& objectsToRender)
{
	// Check for collision between node and frustum
	bool collision = frustum.Intersects(node->nodeBoundingBox);

	if (!collision)
		return; // No collision

	if (node->IsLeaf())
	{
		// Loop over all elements in leaf node
		for (int i = 0; i < node->nElements; i++)
		{
			// Check for collision between element and frustum
			collision = frustum.Intersects(node->elementBoundingBoxes[i]);
			if (collision)
			{
				// Check if element allready added to render list
				bool exists = std::find(std::begin(objectsToRender), std::end(objectsToRender), node->elements[i]) != std::end(objectsToRender);
				if (!exists)
					objectsToRender.push_back(node->elements[i]);
			}
		}
	}
	else
		for (int i = 0; i < 4; i++)
			CullFrustum(frustum, node->children[i], objectsToRender);
}

template<typename T>
inline void QuadTree<T>::AddElement(const T* elementAddress, DX::BoundingBox boundingBox)
{
	AddElement(elementAddress, boundingBox, root, 0);
}

template<typename T>
inline void QuadTree<T>::AddElement(const T* elementAddress, DX::BoundingBox boundingBox, std::unique_ptr<Node>& startNode, int depth)
{
	if (depth >= TREE_MAX_DEPTH)
	{
		DebugLog("Warning: QuadTree maximu depth reached! This may result in undefined behaviour.");
		return; // Max Depth Reached
	}

	bool collision = boundingBox.Intersects(startNode->nodeBoundingBox);

	if (!collision)
		return; // No collision

	if (startNode->IsLeaf())
	{
		if (startNode->IsFull())
		{
			// Leaf is full - Expand tree

			// Get Information for Sub Boxes
			DX::XMFLOAT3 corners[8];
			startNode->nodeBoundingBox.GetCorners(corners);


			DX::XMFLOAT3 max = corners[2];
			DX::XMFLOAT3 min = corners[4];

			// Get center
			DX::XMFLOAT3 parentCenter(0, 0, 0); // Function for getting center???

			for (int i = 0; i < 8; i++)
			{
				parentCenter.x += corners[i].x;
				parentCenter.y += corners[i].y;
				parentCenter.z += corners[i].z;
			}

			parentCenter.x /= 8;
			parentCenter.y /= 8;
			parentCenter.z /= 8;

			// Create Sub Boxes

			// Upper Left
			DX::XMFLOAT3 ulMax(parentCenter.x, max.y, max.z);
			DX::XMFLOAT3 ulMin(min.x, min.y, parentCenter.z);
			startNode->children[UpperLeft] = std::make_unique<Node>();
			DX::BoundingBox bb;
			DX::BoundingBox::CreateFromPoints(bb, DX::XMLoadFloat3(&ulMin), DX::XMLoadFloat3(&ulMax));
			startNode->children[UpperLeft]->nodeBoundingBox = bb;

			// Upper Right
			DX::XMFLOAT3 urMin(parentCenter.x, min.y, parentCenter.z);
			startNode->children[UpperRight] = std::make_unique<Node>();
			DX::BoundingBox::CreateFromPoints(bb, DX::XMLoadFloat3(&urMin), DX::XMLoadFloat3(&max));
			startNode->children[UpperRight]->nodeBoundingBox = bb;

			// Lower Left
			DX::XMFLOAT3 llMax(parentCenter.x, max.y, parentCenter.z);
			startNode->children[LowerLeft] = std::make_unique<Node>();
			DX::BoundingBox::CreateFromPoints(bb, DX::XMLoadFloat3(&min), DX::XMLoadFloat3(&llMax));
			startNode->children[LowerLeft]->nodeBoundingBox = bb;

			// Lower Right
			DX::XMFLOAT3 lrMax(max.x, max.y, parentCenter.z);
			DX::XMFLOAT3 lrMin(parentCenter.x, min.y, min.z);
			startNode->children[LowerRight] = std::make_unique<Node>();
			DX::BoundingBox::CreateFromPoints(bb, DX::XMLoadFloat3(&lrMin), DX::XMLoadFloat3(&lrMax));
			startNode->children[LowerRight]->nodeBoundingBox = bb;

			// Add existing elemnts to newly created leaf nodes
			for (int i = 0; i < LEAF_MAX_OBJECTS; i++)
			{
				// Check each leaf node
				for (int j = 0; j < 4; j++)
					AddElement(startNode->elements[i], startNode->elementBoundingBoxes[i], startNode->children[j], depth+1);
				startNode->elements[i] = nullptr;
			}
			startNode->nElements = 0;

			// Add current element to newly created leaf nodes
			for (int i = 0; i < 4; i++)
				AddElement(elementAddress, boundingBox, startNode->children[i], depth+1);

			return;
		}
		else
		{
			// Add boject and volume to node
			int elementIndex = startNode->nElements++;
			startNode->elements[elementIndex] = elementAddress;
			startNode->elementBoundingBoxes[elementIndex] = boundingBox;
			return;
		}
	}

	// Continue traversing tree
	for (int i = 0; i < 4; i++)
		AddElement(elementAddress, boundingBox, startNode->children[i], depth+1);
}