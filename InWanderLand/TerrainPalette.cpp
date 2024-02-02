#pragma once
#include "InWanderLand.h"
#include "Palette.h"
#include "TerrainPalette.h"
#include "SingletonClass.h"
#include "DebugMeshes.h"
#include "TerrainBrush.h"
#include "SingleNavigationField.h"
#include "TerrainData.h"
#include "InstanceManager.h"
#include "TerrainData.h"

namespace application::editor::palette
{
    using namespace application::editor;
    IEditableData* TerrainPalette::PlaceInstance(Vector3d worldPosition)
    {
        auto centerNodeKey = TerrainData::Instance().WorldToNodeSpace(worldPosition);
        for (int x = centerNodeKey.x - brushSize; x <= centerNodeKey.x + brushSize; x++)
        {
            for (int y = centerNodeKey.y - brushSize; y <= centerNodeKey.y + brushSize; y++)
            {
                auto nodeKey = Vector2i{ x, y };
                if (isMarking)
                {
                    TerrainData::Instance().AddNode(nodeKey, TerrainData::Node{ 0, nullptr });
                }
                else
                {
                    TerrainData::Instance().EraseNode(nodeKey);
                }
            }
        }
        return nullptr;
    }
    void TerrainPalette::OnMouseMove(Vector3d projectedWorldPos)
    {
        Palette::OnMouseMove(projectedWorldPos);
        // 브러시 움직이기
        TerrainBrush::Instance().GetTransform()->SetWorldPosition(TerrainData::Instance().GetNodePosition(TerrainData::Instance().WorldToNodeSpace(projectedWorldPos)));
        if (IsClickingLeft() && !IsSelectMode())
            PlaceInstance(projectedWorldPos);
    }
    void TerrainPalette::SetAsSelectMode(bool isSelectMode)
    {
        Palette::SetAsSelectMode(isSelectMode);
        TerrainBrush::Instance().GetGameObject()->SetSelfActive(!isSelectMode);
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
    void TerrainPalette::ApplyAsPlaytimeObjects()
    {
        TerrainData::Instance().ApplyAsPaletteInstance();
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
        TerrainBrush::Instance().GetGameObject()->SetSelfActive(false);
        CleanUpData();
    }
}