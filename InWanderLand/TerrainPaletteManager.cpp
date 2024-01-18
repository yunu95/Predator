#pragma once
#ifdef EDITOR
#include "InWanderLand.h"
#include "PaletteManager.h"
#include "TerrainPaletteManager.h"
#include "SingletonClass.h"
#include "DebugMeshes.h"
#include "TerrainBrush.h"
#include "SingleNavigationField.h"

namespace application::editor::palette
{
    PaletteInstance* TerrainPaletteManager::PlaceInstance(Vector3d worldPosition)
    {
        auto centerNodeKey = WorldToNodeSpace(worldPosition);
        for (int x = centerNodeKey.x - brushSize; x <= centerNodeKey.x + brushSize; x++)
        {
            for (int y = centerNodeKey.y - brushSize; y <= centerNodeKey.y + brushSize; y++)
            {
                auto nodeKey = Vector2i{ x, y };
                if (isMarking)
                {
                    if (nodes.find(nodeKey) != nodes.end())
                        continue;
                    nodes[nodeKey] = CreateNodeDebuggingMesh(nodeKey);
                }
                else
                {
                    if (nodes.find(nodeKey) == nodes.end())
                        continue;
                    Scene::getCurrentScene()->DestroyGameObject(nodes[nodeKey]);
                    nodes.erase(nodeKey);
                }
            }
        }
        return nullptr;
    }
    void TerrainPaletteManager::OnMouseMove(Vector3d projectedWorldPos)
    {
        PaletteManager::OnMouseMove(projectedWorldPos);
        // 브러시 움직이기
        TerrainBrush::Instance().GetTransform()->SetWorldPosition(GetNodePosition(WorldToNodeSpace(projectedWorldPos)));
        if (IsClickingLeft() && !IsSelectMode())
            PlaceInstance(projectedWorldPos);
    }
    void TerrainPaletteManager::SetAsSelectMode(bool isSelectMode)
    {
        PaletteManager::SetAsSelectMode(isSelectMode);
        TerrainBrush::Instance().SetActive(!isSelectMode);
    }
    bool TerrainPaletteManager::IsMarking()
    {
        return isMarking;
    }
    void TerrainPaletteManager::SetIsMarking(bool isMarking)
    {
        this->isMarking = isMarking;
    }
    // 브러시 크기는 0부터 시작합니다.
    unsigned int TerrainPaletteManager::GetBrushSize()
    {
        return brushSize;
    }
    void TerrainPaletteManager::SetBrushSize(unsigned int brushSize)
    {
        this->brushSize = brushSize;
        TerrainBrush::Instance().SetBrushSize(brushSize);
    }
    void TerrainPaletteManager::ApplyAsPlaytimeObjects()
    {
        std::vector<Vector3f> vertexList;
        std::vector<int> indexList;
        for (auto& node : nodes)
        {
            auto nodeKey = node.first;
            bool toprightExists{ nodes.find(nodeKey + Vector2i{1, 1}) != nodes.end() };
            bool topExists{ nodes.find(nodeKey + Vector2i{0, 1}) != nodes.end() };
            bool rightExists{ nodes.find(nodeKey + Vector2i{1, 0}) != nodes.end() };
            if (toprightExists && topExists && rightExists)
            {
                vertexList.push_back(GetNodePosition(nodeKey));
                vertexList.push_back(GetNodePosition(nodeKey + Vector2i{1, 0}));
                vertexList.push_back(GetNodePosition(nodeKey + Vector2i{0, 1}));
                vertexList.push_back(GetNodePosition(nodeKey + Vector2i{1, 1}));
                indexList.push_back(vertexList.size() - 2);
                indexList.push_back(vertexList.size() - 3);
                indexList.push_back(vertexList.size() - 4);
                indexList.push_back(vertexList.size() - 2);
                indexList.push_back(vertexList.size() - 1);
                indexList.push_back(vertexList.size() - 3);
            }
            else if (toprightExists && topExists)
            {
                vertexList.push_back(GetNodePosition(nodeKey));
                vertexList.push_back(GetNodePosition(nodeKey + Vector2i{1, 1}));
                vertexList.push_back(GetNodePosition(nodeKey + Vector2i{0, 1}));
                indexList.push_back(vertexList.size() - 1);
                indexList.push_back(vertexList.size() - 2);
                indexList.push_back(vertexList.size() - 3);
            }
            else if (toprightExists && rightExists)
            {
                vertexList.push_back(GetNodePosition(nodeKey));
                vertexList.push_back(GetNodePosition(nodeKey + Vector2i{1, 1}));
                vertexList.push_back(GetNodePosition(nodeKey + Vector2i{1, 0}));
                indexList.push_back(vertexList.size() - 3);
                indexList.push_back(vertexList.size() - 2);
                indexList.push_back(vertexList.size() - 1);
            }
            else if (topExists && rightExists)
            {
                vertexList.push_back(GetNodePosition(nodeKey));
                vertexList.push_back(GetNodePosition(nodeKey + Vector2i{0, 1}));
                vertexList.push_back(GetNodePosition(nodeKey + Vector2i{1, 0}));
                indexList.push_back(vertexList.size() - 3);
                indexList.push_back(vertexList.size() - 2);
                indexList.push_back(vertexList.size() - 1);
            }
        }
        SingleNavigationField::Instance().BuildField(vertexList, indexList);
    }
    Vector3d TerrainPaletteManager::GetNodePosition(const Vector2i& nodeKey)
    {
        return { nodeKey.x * nodeDistance , 0 ,nodeKey.y * nodeDistance };
    }
    Vector2i TerrainPaletteManager::WorldToNodeSpace(const Vector3d& worldPos)
    {
        return { (int)(0.5 + worldPos.x / nodeDistance) , (int)(0.5 + worldPos.z / nodeDistance) };
    }
    GameObject* TerrainPaletteManager::CreateNodeDebuggingMesh(const Vector2i& nodeKey)
    {
        auto node = Scene::getCurrentScene()->AddGameObject();
        auto mesh = AttachDebugMesh(node, DebugMeshType::Cube, yunuGI::Color{0.788, 0.647, 0.215}, false);
        mesh->SetIsUpdating(false);
        node->GetTransform()->scale = { nodeDistance, nodeHeight, nodeDistance };
        node->GetTransform()->SetWorldPosition(GetNodePosition(nodeKey) - nodeHeight * Vector3d::up * 0.5);
        return node;
    }
}
#endif