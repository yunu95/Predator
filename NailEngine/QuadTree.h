#pragma once

#include <vector>
#include <unordered_map>

#include <DirectXMath.h>
#include "SimpleMath.h"
using namespace DirectX::PackedVector;
using namespace DirectX::SimpleMath;

template <typename DataType>
class QuadTree
{
private:
	struct DataSet
	{
		Vector2 pos;
		float radius;
		DataType* dataPtr;
	};

	struct Node
	{
		Node(Vector2 minPoint, Vector2 maxPoint, Node* parent = nullptr)
			: minPoint(minPoint), maxPoint(maxPoint), maxRadius(0.f), parent(parent)
		{
			for (int i = 0; i < 4; i++)
			{
				children[i] = nullptr;
			}
		};

		Node* parent;
		Node* children[4];

		std::vector<DataSet> dataVec;

		Vector2 minPoint;
		Vector2 maxPoint;

		float maxRadius;

		bool IsLeafNode() const
		{
			return dataVec.size() > 0;
		}
	};

public:
	// 쿼드트리 범위 지정

	// 최소 노드 크기 지정

	QuadTree(Vector2 treeSize, Vector2 minNodeSize)
	{
		this->treeSize = treeSize;
		this->minAreaSize = minNodeSize;

		this->root = new Node(-this->treeSize / 2.f, this->treeSize / 2.f, nullptr);
	}

	~QuadTree()
	{
		DestroyRecursive(root);
	}

	// 데이터 밀어 넣을 때 
	void PushData(DataType* data, Vector2 pos, float radius)
	{
		PushDataRecursive(data, pos, radius, root);
	}

	// 특정 데이터 지우는 함수
	void Remove(DataType* dataPtr)
	{
		assert(nodeTable.count(dataPtr) == 1); // 없는 데이터를 삭제하려 함
		Node* node = nodeTable[dataPtr];

		for (size_t i = 0; i < node->dataVec.size(); i++)
		{
			if (node->dataVec[i].dataPtr == dataPtr)
			{
				node->dataVec.erase(node->dataVec.begin() + i);
				assert(nodeTable.count(dataPtr) != 0);
				nodeTable.erase(dataPtr);

				UpdateRadiusRecursive(node);

				ClearLeafNode(node);
				return;
			}
		}

		// 못찾았으면 문제가 있는 것임
		assert(0);
		return;
	}

	// 오브젝트의 포지션이 변경 될 때 ( 현재 오브젝트가 포함된 노드가 바뀔 수 있음 )
	void UpdatePosition(Vector2 pos, float radius, DataType* dataPtr)
	{
		assert(nodeTable.count(dataPtr) == 1); // 없는 데이터를 이동하려 함
		Node* node = nodeTable[dataPtr];

		bool isIntersects = Intersects(pos, pos, node->minPoint, node->maxPoint);

		if (isIntersects)
		{
			// 현재 노드라면 바로 이동
			for (size_t i = 0; i < node->dataVec.size(); i++)
			{
				if (node->dataVec[i].dataPtr == dataPtr)
				{
					node->dataVec[i].pos = pos;
					node->dataVec[i].radius = radius;

					// 이동 종료
					return;
				}
			}

			// 못찾았으면 문제가 있는 것임
			assert(0);
			return;
		}
		else
		{
			// Insert의 assert(nodeTable.count(dataPtr) == 0)에서 중단되는 경우가 가끔 발생한다.
			// Remove에서 데이터를 삭제하지 못한 경우로 예상되는데 확인이 필요하다... -> dataPtr로 삭제하는거로 삭제하게 함 아마 고쳐진듯?

			Remove(dataPtr);
			PushData(dataPtr, pos, radius);
		}
	}

	// 외부에서 쿼리하면 데이터들을 다 넘겨줌
	std::vector<DataType*> QueryData(Vector2 minPoint, Vector2 maxPoint)
	{
		std::vector<DataType*> result;
		QueryRecursive(minPoint, maxPoint, root, result);
		return result;
	}

private:

	// 재귀적으로 dataVec에 값을 넣어줌
	void QueryRecursive(Vector2& minPoint, Vector2& maxPoint, Node* node, std::vector<DataType*>& result)
	{
		// node의 영역이 node안의 원소의 최대 반지름 크기만큼 확장된 영역과 탐색 영역이 겹치지 않는다면 종료
		if (!Intersects(minPoint, maxPoint, node->minPoint - Vector2(node->maxRadius, node->maxRadius), node->maxPoint + Vector2(node->maxRadius, node->maxRadius)))
		{
			return;
		}

		// LeafNode라면 각각의 원소의 반지름으로 조금 더 정확한 판정을 하여 결과에 추가
		if (node->IsLeafNode())
		{
			for (const DataSet& data : node->dataVec)
			{
				if (Intersects(minPoint, maxPoint, (data.pos - Vector2(data.radius, data.radius)), data.pos + Vector2(data.radius, data.radius)))
				{
					result.push_back(data.dataPtr);
				}
			}
			return;
		}

		// 그 외의 경우에 모든 자식 노드들에 대해서 재귀호출
		for (int i = 0; i < 4; i++)
		{
			if (node->children[i] != nullptr)
			{
				QueryRecursive(minPoint, maxPoint, node->children[i], result);
			}
		}
	}


	// 현재 노드가 어느 사분면에 포함되는지를 알려주는 함수
	int GetQuadrant(Vector2 pos, Vector2 minPoint, Vector2 maxPoint)
	{
		int quadrant = 0;
		Vector2 midPoint = (minPoint + maxPoint) * 0.5f;

		if (((maxPoint.x - minPoint.x) * 0.5f >= minAreaSize.x) && (pos.x > midPoint.x))
		{
			quadrant |= 1;
		}

		if (((maxPoint.y - minPoint.y) * 0.5f >= minAreaSize.y) && (pos.y > midPoint.y))
		{
			quadrant |= 2;
		}

		return quadrant;
	}

	// 재귀적으로 데이터를 넣는 함수 ( 노드가 없으면 생성함 )
	void PushDataRecursive(DataType* data, Vector2 pos, float radius, Node* node)
	{
		Vector2 minPoint = node->minPoint;
		Vector2 maxPoint = node->maxPoint;

		// pos가 영역 밖에 있는 경우 종료
		if (!Intersects(pos, pos, node->minPoint, node->maxPoint))
		{
			return;
		}

		// LeafNode면 데이터 삽입 후 종료
		if (node->IsLeafNode())
		{
			node->dataVec.push_back({ pos,radius,data });

			UpdateRadiusRecursive(node);

			assert(nodeTable.count(data) == 0);
			nodeTable[data] = node;
			return;
		}

		// 더 이상 분할 하면 최소 크기보다 작아지는 경우 데이터 삽입 후 종료
		if (((maxPoint.x - minPoint.x) * 0.5f < minAreaSize.x) && ((maxPoint.y - minPoint.y) * 0.5f < minAreaSize.y))
		{
			node->dataVec.push_back({ pos,radius,data });
			UpdateRadiusRecursive(node);
			assert(nodeTable.count(data) == 0);
			nodeTable[data] = node;
			return;
		}

		int quadrant = GetQuadrant(pos, minPoint, maxPoint);

		Vector2 childMinPoint = GetQuadrantMinPoint(quadrant, pos, minPoint, maxPoint);
		Vector2 childMaxPoint = GetQuadrantMaxPoint(quadrant, pos, minPoint, maxPoint);

		// 찾고 있는 quadrant가 없다면 새로운 자식 노드를 만든다.
		if (node->children[quadrant] == nullptr)
		{
			node->children[quadrant] = new Node(childMinPoint, childMaxPoint, node);
		}

		// 재귀호출로 자식 노드에 데이터를 삽입한다.
		PushDataRecursive(data, pos, radius, node->children[quadrant]);
	}

	// 두 사각형이 주어질 때 겹치는 영역이 있는지 판단하는 함수 ( 겹치면 true 반환 )
	bool Intersects(Vector2 minPoint1, Vector2 maxPoint1, Vector2 minPoint2, Vector2 maxPoint2)
	{
		Vector2 rootMinPoint = -this->treeSize / 2.f;
		Vector2 rootMaxPoint = this->treeSize / 2.f;

		if (maxPoint1.x < rootMinPoint.x) maxPoint1.x = rootMinPoint.x;
		if (maxPoint1.y < rootMinPoint.y) maxPoint1.y = rootMinPoint.y;

		if (rootMaxPoint.x < minPoint1.x) minPoint1.x = rootMaxPoint.x;
		if (rootMaxPoint.y < minPoint1.y) minPoint1.y = rootMaxPoint.y;

		if (maxPoint2.x < rootMinPoint.x) maxPoint2.x = rootMinPoint.x;
		if (maxPoint2.y < rootMinPoint.y) maxPoint2.y = rootMinPoint.y;

		if (rootMaxPoint.x < minPoint2.x) minPoint2.x = rootMaxPoint.x;
		if (rootMaxPoint.y < minPoint2.y) minPoint2.y = rootMaxPoint.y;

		if (maxPoint1.x < minPoint2.x || minPoint1.x > maxPoint2.x)
			return false;

		if (maxPoint1.y < minPoint2.y || minPoint1.y > maxPoint2.y)
			return false;

		return true;
	}

	Vector2 GetQuadrantMinPoint(int quadrant, Vector2 point, const Vector2 minPoint, const Vector2 maxPoint) const
	{
		bool holdx = (maxPoint.x - minPoint.x) * 0.5f < minAreaSize.x;
		bool holdy = (maxPoint.y - minPoint.y) * 0.5f < minAreaSize.y;

		Vector2 quadrantMinPoint = Vector2(
			(quadrant & 1) ? (minPoint.x + maxPoint.x) * 0.5f : minPoint.x,
			(quadrant & 2) ? (minPoint.y + maxPoint.y) * 0.5f : minPoint.y);

		if (holdx) quadrantMinPoint.x = minPoint.x;
		if (holdy) quadrantMinPoint.y = minPoint.y;

		return quadrantMinPoint;
	}

	Vector2 GetQuadrantMaxPoint(int quadrant, const Vector2 point, const Vector2 minPoint, const Vector2 maxPoint) const
	{
		bool holdx = (maxPoint.x - minPoint.x) * 0.5f < minAreaSize.x;
		bool holdy = (maxPoint.y - minPoint.y) * 0.5f < minAreaSize.y;

		Vector2 quadrantMaxPoint = Vector2(
			(quadrant & 1) ? maxPoint.x : (minPoint.x + maxPoint.x) * 0.5f,
			(quadrant & 2) ? maxPoint.y : (minPoint.y + maxPoint.y) * 0.5f);

		if (holdx) quadrantMaxPoint.x = maxPoint.x;
		if (holdy) quadrantMaxPoint.y = maxPoint.y;

		return quadrantMaxPoint;
	}

	// 최대 반지름 리턴
	float UpdateAllRadiusRecursive(Node* node)
	{
		float result = 0.0f;

		// 리프노드라면 모든 데이터의 반지름을 확인하여 최댓값을 node에 저장하고 리턴
		if (node->IsLeafNode())
		{
			for (size_t i = 0; i < node->dataVec.size(); i++)
			{
				float currentradius = node->dataVec[i]->radius;
				if (result < currentradius)
				{
					result = currentradius;
				}
			}

			node->maxRadius = result;

			return result;
		}

		// 모든 자식 노드의 값으로 최댓값을 찾는다.
		for (size_t i = 0; i < 4; i++)
		{
			if (node->children[i] == nullptr)
			{
				continue;
			}

			float currentradius = UpdateAllRadiusRecursive(node->children[i]);
			if (result < currentradius)
			{
				result = currentradius;
			}
		}

		node->maxRadius = result;

		return result;
	}

	void UpdateRadiusRecursive(Node* node)
	{
		if (node == nullptr)
		{
			return;
		}

		float result = 0.0f;

		// 리프노드라면 모든 데이터의 반지름을 확인하여 최댓값을 구한다.
		if (node->IsLeafNode())
		{
			for (size_t i = 0; i < node->dataVec.size(); i++)
			{
				float currentradius = node->dataVec[i].radius;
				if (result < currentradius)
				{
					result = currentradius;
				}
			}
		}
		// 리프노드가 아니라면 모든 자식 노드에서 최댓값 확인
		else
		{
			for (size_t i = 0; i < 4; i++)
			{
				float currentradius = 0.0f;
				if (node->children[i] == nullptr) continue;

				currentradius = node->children[i]->maxRadius;

				if (result < currentradius)
				{
					result = currentradius;
				}
			}
		}

		float prevMax = node->maxRadius;
		node->maxRadius = result;

		// maxRadius가 변경됐다면 parentNode도 업데이트 한다.
		if (prevMax != result)
		{
			UpdateRadiusRecursive(node->parent);
		}
	}

	void ClearLeafNode(Node* node)
	{
		// 리프노드라면 문제 없음
		if (node->IsLeafNode())
		{
			return;
		}

		// 자식 노드의 개수를 확인한다
		int childCount = 0;
		for (size_t i = 0; i < 4; i++)
		{
			if (node->children[i] != nullptr)
			{
				childCount++;
			}
		}

		// 자식노드가 없다면 데이터가 삭제된 리프노드이므로 제거한다. 제거 후 그 부모 오브젝트도 확인해야 한다.
		if (childCount == 0)
		{
			Node* parent = node->parent;
			DestroyRecursive(node);
			ClearLeafNode(parent);
		}
	}

	void DestroyRecursive(Node* node)
	{
		assert(node != nullptr);
		if (node != nullptr) return;

		// root 노드가 아닌경우 parentNode를 따라가서 children에서 자신을 제거
		if (node->parent != nullptr)
		{
			for (size_t i = 0; i < 4; i++)
			{
				if (node->parent->children[i] == node)
				{
					node->parent->children[i] = nullptr;
				}
			}
		}
		// root 노드인경우 root = nullptr
		else
		{
			root = nullptr;
		}

		// nullptr이 아닌 모든 자식 노드에 대해서 DestroyRecursive
		for (int i = 0; i < 4; i++)
		{
			if (node->children[i] != nullptr)
			{
				DestroyRecursive(node->children[i]);
			}
		}

		delete node;
	}

private:
	Node* root = nullptr;
	// 트리의 크기
	Vector2 treeSize{ 0,0 };
	// 노드의 최소 크기
	Vector2 minAreaSize{ 0,0 };
	std::unordered_map<DataType*, Node*> nodeTable;
};

