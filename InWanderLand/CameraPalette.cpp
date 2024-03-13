#include "CameraPalette.h"
#include "WavePalette.h"
#include "CameraEditorInstance.h"
#include "SelectionBox.h"
#include "InstanceManager.h"
#include "Camera_TemplateData.h"
#include "CameraData.h"
#include "TemplateDataManager.h"
#include "CameraBrush.h"

namespace application::editor::palette
{
    void CameraPalette::Initialize()
    {
        TemplateDataManager::GetSingletonInstance().CreateTemplateData<Camera_TemplateData>("DefaultCamera");
    }

    void CameraPalette::Reset()
    {
        Palette::Reset();
        CameraBrush::Instance().ReadyBrush("");
    }

    CameraData* CameraPalette::PlaceInstance(Vector3d worldPosition)
    {
        auto instance = InstanceManager::GetSingletonInstance().CreateInstance<CameraData>("DefaultCamera");
        instance->pod.position.x = worldPosition.x;
        instance->pod.position.y = worldPosition.y;
        instance->pod.position.z = worldPosition.z;

        instance->ApplyAsPaletteInstance();
        return instance;
    }

    void CameraPalette::OnMouseMove(Vector3d projectedWorldPos, Vector2d normalizedScreenPos)
    {
        Palette::OnMouseMove(projectedWorldPos, normalizedScreenPos);
        // 브러시 움직이기
        CameraBrush::Instance().GetTransform()->SetWorldPosition(projectedWorldPos);
        if (IsClickingLeft() && !IsSelectMode() && CheckInstantiationCooltime())
            PlaceInstance(projectedWorldPos);
    }

    void CameraPalette::SetAsSelectMode(bool isSelectMode)
    {
        Palette::SetAsSelectMode(isSelectMode);
        if (isSelectMode)
        {
            CameraBrush::Instance().ReadyBrush("");
        }
        else
        {
            CameraBrush::Instance().ReadyBrush("DefaultCamera");
        }
    }

    bool CameraPalette::ShouldSelect(IEditableData* instance)
    {
        return dynamic_cast<CameraData*>(instance);
    }

    void CameraPalette::OnStartPalette()
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
    }

    void CameraPalette::OnStandbyPalette()
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
        CleanUpData();
        WavePalette::SingleInstance().currentSelectedWaveIndex = -1;
    }

    void CameraPalette::CleanUpData()
    {
        Palette::CleanUpData();
        Reset();
    }
}
