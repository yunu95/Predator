#pragma once
#include "YunutyEngine.h"
using namespace yunutyEngine;
// RTS 경로를 XZ평면에 생성하기 위한 컴포넌트
// 이 컴포넌트가 포함된 XZ 평면을 그리드로 나누고, 그리드의 꼭짓점에 경로 노드를 지정할 수 있게 한다.
// 인접한 경로노드들끼리 이루는 다각형은 RTS 유닛들이 오갈 수 있는 경로가 된다.
class PathField : public Component
{
public:
    struct Node
    {
        bool passable{ false };
        graphics::StaticMeshRenderer* debugMesh{nullptr};
    };
    // 가로,세로로 노드 테이블을 만든다.
    void Initialize(int xSize, int ySize, float nodeWidth)
    {
        this->NodesXSize = xSize;
        this->NodesYSize = ySize;
        this->nodeWidth = nodeWidth;
        this->initializedWorldPos = GetTransform()->GetWorldPosition();
        nodeTable.resize(xSize, std::vector<Node>(ySize));
    }
    Node* GetNode(const Vector3d& position)
    {
        Vector2i nodeIndex = NodePositionToIndex(position);
        if (nodeIndex.x < 0 || nodeIndex.y < 0 || nodeIndex.x >= NodesXSize || nodeIndex.y >= NodesYSize)
            return nullptr;
        return &nodeTable[nodeIndex.x][nodeIndex.y];
    }
    Vector3d GetNearestNodePosition(const Vector3d& position)
    {
        auto nodeIndex = NodePositionToIndex(position);
        return NodePosition(nodeIndex.x, nodeIndex.y);
    }
    // 노드를 표시할 때에 쓰이는 디버그 메시
    static graphics::StaticMeshRenderer* CreateNodeDebugMesh()
    {
        auto nodeHoverDebugMesh = Scene::getCurrentScene()->AddGameObject()->AddComponent<graphics::StaticMeshRenderer>();
        nodeHoverDebugMesh->GetTransform()->scale = Vector3d{ 0.2,1,0.2 };
        nodeHoverDebugMesh->GetGI().LoadMesh("Cylinder");
        nodeHoverDebugMesh->GetGI().GetMaterial()->SetColor(yunuGI::Color::white());
        nodeHoverDebugMesh->GetGI().GetMaterial()->SetPixelShader(L"DefaultPS.cso");

        return nodeHoverDebugMesh;
    }
private:
    std::vector<std::vector<Node>> nodeTable;
    float nodeWidth;
    // 경로 필드가 초기화되었을 때 필드의 월드 위치
    Vector3d initializedWorldPos;
    int NodesXSize{ 0 };
    int NodesYSize{ 0 };
    Vector3d NodePosition(int x, int y)
    {
        return initializedWorldPos + Vector3d{(x - NodesXSize / 2)* nodeWidth, 0, (y - NodesYSize / 2)* nodeWidth};
    }
    Vector2i NodePositionToIndex(Vector3d position)
    {
        Vector3d relativePos = (position - initializedWorldPos) / nodeWidth;
        return Vector2i{ static_cast<int>(relativePos.x + NodesXSize / 2 + 0.5),static_cast<int>(relativePos.z + NodesYSize / 2 + 0.5) };
    }
};
