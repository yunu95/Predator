#pragma once
#include "InWanderLand.h"
#include "Palette.h"
#include "TerrainPalette.h"
#include "SingletonClass.h"
#include "DebugMeshes.h"
#include "TerrainBrush.h"
#include "SingleNavigationField.h"

namespace application::editor::palette
{
    PaletteInstance* TerrainPalette::PlaceInstance(Vector3d worldPosition)
    {
        auto centerNodeKey = WorldToNodeSpace(worldPosition);
        for (int x = centerNodeKey.x - brushSize; x <= centerNodeKey.x + brushSize; x++)
        {
            for (int y = centerNodeKey.y - brushSize; y <= centerNodeKey.y + brushSize; y++)
            {
                auto nodeKey = Vector2i{ x, y };
                if (isMarking)
                {
                    AddNode(nodeKey);
                }
                else
                {
                    EraseNode(nodeKey);
                }
            }
        }
        return nullptr;
    }
    void TerrainPalette::OnMouseMove(Vector3d projectedWorldPos)
    {
        Palette::OnMouseMove(projectedWorldPos);
        // 브러시 움직이기
        TerrainBrush::Instance().GetTransform()->SetWorldPosition(GetNodePosition(WorldToNodeSpace(projectedWorldPos)));
        if (IsClickingLeft() && !IsSelectMode())
            PlaceInstance(projectedWorldPos);
    }
    void TerrainPalette::SetAsSelectMode(bool isSelectMode)
    {
        Palette::SetAsSelectMode(isSelectMode);
        TerrainBrush::Instance().SetActive(!isSelectMode);
    }
    bool TerrainPalette::IsMarking()
    {
        return isMarking;
    }
    void TerrainPalette::SetIsMarking(bool isMarking)
    {
        this->isMarking = isMarking;
    }
    // 브러시 크기는 0부터 시작합니다.
    unsigned int TerrainPalette::GetBrushSize()
    {
        return brushSize;
    }
    void TerrainPalette::SetBrushSize(unsigned int brushSize)
    {
        this->brushSize = brushSize;
        TerrainBrush::Instance().SetBrushSize(brushSize);
    }
    void TerrainPalette::AddNode(const Vector2i& nodeKey)
    {
        if (nodes.find(nodeKey) != nodes.end())
            return;
#ifdef _DEBUG
        nodes[nodeKey] = CreateNodeDebuggingMesh(nodeKey);
#else
        nodes[nodeKey] = nullptr;
#endif
    }
    void TerrainPalette::EraseNode(const Vector2i& nodeKey)
    {
        if (nodes.find(nodeKey) == nodes.end())
            return;
#ifdef _DEBUG
        Scene::getCurrentScene()->DestroyGameObject(nodes[nodeKey]);
#endif
        nodes.erase(nodeKey);
    }
    void TerrainPalette::ClearNodes()
    {
        for (auto& node : nodes)
        {
#ifdef _DEBUG
            Scene::getCurrentScene()->DestroyGameObject(node.second);
#endif
        }
        nodes.clear();
    }
    vector<Vector2i> TerrainPalette::GetNodePositionList()
    {
        vector<Vector2i> ret;
        ret.reserve(nodes.size());
        for (auto& node : nodes)
        {
            ret.push_back(node.first);
        }
        return ret;
    }
    void TerrainPalette::ApplyAsPlaytimeObjects()
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
    void TerrainPalette::OnStartPalette()
    {
        switch (beforeState)
        {
        case application::editor::palette::Palette::State::Place:
        {
            SetAsSelectMode(false);
            break;
        }
        default:
            SetAsSelectMode(true);
            break;
        }
        SetBrushSize(2);
    }
    void TerrainPalette::OnStandbyPalette()
    {
        if (IsSelectMode())
        {
            beforeState = State::Select;
        }
        else
        {
            beforeState = State::Place;
        }
        state = State::None;
        TerrainBrush::Instance().SetActive(false);
        CleanUpData();
    }
    Vector3d TerrainPalette::GetNodePosition(const Vector2i& nodeKey)
    {
        return { nodeKey.x * nodeDistance , 0 ,nodeKey.y * nodeDistance };
    }
    Vector2i TerrainPalette::WorldToNodeSpace(const Vector3d& worldPos)
    {
        return { (int)(0.5 + worldPos.x / nodeDistance) , (int)(0.5 + worldPos.z / nodeDistance) };
    }
    GameObject* TerrainPalette::CreateNodeDebuggingMesh(const Vector2i& nodeKey)
    {
        auto node = Scene::getCurrentScene()->AddGameObject();
        node->GetTransform()->SetWorldPosition(GetNodePosition(nodeKey) - nodeHeight * Vector3d::up * 0.5);
        node->GetTransform()->scale = { nodeDistance, nodeHeight, nodeDistance };
        auto mesh = AttachDebugMesh(node, DebugMeshType::Cube, yunuGI::Color{0.788, 0.647, 0.215}, false);
        mesh->SetIsUpdating(false);
        return node;
    }
}