#include "InWanderLand.h"
#include "Palette.h"
#include "IEditableData.h"
#include "InstanceManager.h"
#include "SelectionBox.h"
#include "PaletteInstance.h"
#include "RegionPalette.h"
#include "UnitPalette.h"
#include "TerrainPalette.h"
#include "OrnamentPalette.h"
#include "RegionPalette.h"
#include "WavePalette.h"
#include "Panel_SceneView.h"

namespace application::editor::palette
{
    void Palette::ResetPalettes()
    {
        static_cast<Palette&>(UnitPalette::SingleInstance()).Reset();
        static_cast<Palette&>(TerrainPalette::SingleInstance()).Reset();
        static_cast<Palette&>(OrnamentPalette::SingleInstance()).Reset();
        static_cast<Palette&>(RegionPalette::SingleInstance()).Reset();
        static_cast<Palette&>(WavePalette::SingleInstance()).Reset();
    }

    void Palette::Initialize()
    {

    }

    void Palette::OnLeftClick()
    {
        isClickingLeft = true;
        /// 호버링 되는 상태의 객체가 있다면 객체를 단 하나만 선택합니다. 선택된 객체가 없다면 눈치를 보다가 드래깅을 시작합니다.
        switch (state)
        {
        case application::editor::palette::Palette::State::None:
            break;
            /// <summary>
            /// SelectMode에서 좌클릭이 일어났을때, 클릭할 수 있는 객체가 있으면 해당 객체만 선택 리스트로 지정하고 드래깅 모드로 진입합니다.
            /// SelectMode에서 좌클릭이 일어났을때, 클릭할 수 있는 객체가 있는데 이미 걔가 선택리스트에 있으면 그대로 드래깅 모드로 진입합니다.
            /// SelectMode에서 좌클릭이 일어났을 때, 클릭할 수 있는 객체가 없으면 선택박스 모드로 진입합니다.
            /// </summary>
        case application::editor::palette::Palette::State::Select:
            if (pendingSelection)
            {
                OnSelectSingleInstance(pendingSelection);
                lastFrameBrushPos = currentBrushPos;
                UnHoverCurrentInstance();
            }
            else
            {
                OnSelectEmpty();
            }
            break;
        case application::editor::palette::Palette::State::Place:
            lastInstantiationTime = Time::GetTimeElapsedUnscaled();
            PlaceInstance(currentBrushPos);
            break;
            /// DraggingObjects,DraggingSelect 상태는 이미 좌클릭을 하고 있는 상태이기 때문에 아무것도 할 수 없음.
        case application::editor::palette::Palette::State::DraggingObjects:
            break;
        case application::editor::palette::Palette::State::DraggingSelectBox:
            break;
        default:
            break;
        }
    }
    void Palette::OnLeftClickRelease()
    {
        isClickingLeft = false;
        switch (state)
        {
        case application::editor::palette::Palette::State::None:
            break;
        case application::editor::palette::Palette::State::Select:
            break;
        case application::editor::palette::Palette::State::Place:
            break;
        case application::editor::palette::Palette::State::DraggingObjects:
            state = State::Select;
            break;
        case application::editor::palette::Palette::State::DraggingSelectBox:
            //SelectionBox::Instance().ShowSelectionBox(false);
            state = State::Select;
            break;
        default:
            break;
        }
    }
    void Palette::OnMouseMove(Vector3d projectedWorldPos, Vector2d normalizedScreenPos)
    {
        currentBrushPos = projectedWorldPos;
        switch (state)
        {
        case application::editor::palette::Palette::State::None:
            break;
        case application::editor::palette::Palette::State::Select:
            HoverClosestInstance();
            SelectionBox::Instance().SetAsPickingCollider(graphics::Camera::GetMainCamera(), normalizedScreenPos);
            //SelectionBox::Instance().SetCoverage(currentBrushPos - unDraggingHalfExtent, currentBrushPos + unDraggingHalfExtent);
            break;
        case application::editor::palette::Palette::State::Place:

            break;
        case application::editor::palette::Palette::State::DraggingObjects:
            for (auto each : selection)
            {
                //each->OnRelocate(each->GetPaletteInstance()->GetTransform()->GetWorldPosition() + currentBrushPos - lastFrameBrushPos);
                //each->ApplyAsPaletteInstance();
            }
            break;
        case application::editor::palette::Palette::State::DraggingSelectBox:
            SelectionBox::Instance().SetCoverage(dragStartPos, currentBrushPos);
            break;
        default:
            break;
        }
        lastFrameBrushPos = currentBrushPos;
        /// 인스턴스 드래깅 상태라면 드래깅되는 객체들의 위치를 옮깁니다.
    }
    void Palette::OnDeletion()
    {
        for (auto each : selection)
            Delete(each);
        selection.clear();
    }
    void Palette::Delete(IEditableData* data)
    {
        if (data == pendingSelection)
            pendingSelection = nullptr;
        auto& contactingInstances = SelectionBox::Instance().GetContactingInstances();
        contactingInstances.erase(data->GetPaletteInstance());

        InstanceManager::GetSingletonInstance().DeleteInstance(data->GetUUID());
    }
    void Palette::SetAsSelectMode(bool isSelectMode)
    {
        if (isSelectMode)
        {
            state = State::Select;
            SelectionBox::Instance().ShowSelectionBox(true);
        }
        else
        {
            state = State::Place;
            SelectionBox::Instance().ShowSelectionBox(false);
        }
    }
    bool Palette::IsSelectMode()
    {
        return state != State::Place;
    }
    void Palette::OnSelectSingleInstance(IEditableData* data)
    {
        if (!SceneViewPanel::GetSingletonInstance().IsMouseOverGizmo())
        {
            if (selection.find(data) == selection.end())
            {
                ClearSelection();
                InsertSelection(data);
            }
            state = State::DraggingObjects;
        }
    }
    bool Palette::IsClickingLeft()
    {
        return isClickingLeft;
    }

    void Palette::CleanUpData()
    {
        pendingSelection = nullptr;
        ClearSelection();
        draggingObject = nullptr;
        isClickingLeft = false;
    }

    void Palette::OnSelectionContactEnter(IEditableData* instance)
    {
        switch (state)
        {
        case application::editor::palette::Palette::State::None:
            break;
        case application::editor::palette::Palette::State::Select:
            HoverClosestInstance();
            break;
        case application::editor::palette::Palette::State::Place:
            break;
        case application::editor::palette::Palette::State::DraggingObjects:
            break;
        case application::editor::palette::Palette::State::DraggingSelectBox:
            InsertSelection(instance);
            break;
        default:
            break;
        }
    }
    void Palette::OnSelectionContactExit(IEditableData* instance)
    {
        switch (state)
        {
        case application::editor::palette::Palette::State::None:
            break;
        case application::editor::palette::Palette::State::Select:
            HoverClosestInstance();
            break;
        case application::editor::palette::Palette::State::Place:
            break;
        case application::editor::palette::Palette::State::DraggingObjects:
            break;
        case application::editor::palette::Palette::State::DraggingSelectBox:
            EraseSelection(instance);
            break;
        default:
            break;
        }
    }

    void Palette::OnSelectEmpty()
    {
        if (!SceneViewPanel::GetSingletonInstance().IsMouseOverGizmo())
        {
            state = State::DraggingSelectBox;
            dragStartPos = currentBrushPos;
            ClearSelection();
            SelectionBox::Instance().ShowSelectionBox(true);
            SelectionBox::Instance().SetCoverage(dragStartPos, currentBrushPos);
        }
    }

    void Palette::InsertSelection(IEditableData* instance)
    {
        if (selection.find(instance) == selection.end())
        {
            selection.insert(instance);
            instance->GetPaletteInstance()->OnSelected();
        }

    }
    void Palette::EraseSelection(IEditableData* instance)
    {
        if (selection.find(instance) != selection.end())
        {
            selection.erase(instance);
            instance->GetPaletteInstance()->OnDeselected();
        }
    }
    void Palette::ClearSelection()
    {
        for (auto each : selection)
            each->GetPaletteInstance()->OnDeselected();
        selection.clear();
    }
    void Palette::HoverClosestInstance()
    {
        IEditableData* newPendingSelection{ nullptr };
        auto& contactingInstances = SelectionBox::Instance().GetContactingInstances();
        if (contactingInstances.empty())
        {
            newPendingSelection = nullptr;
        }
        else
        {
            for (auto each : contactingInstances)
            {
                if (ShouldSelect(each->GetEditableData()))
                {
                    newPendingSelection = each->GetEditableData();
                    break;
                }
            }
            for (auto each : contactingInstances)
            {
                if (newPendingSelection == nullptr)
                    break;
                if (ShouldSelect(each->GetEditableData()) &&
                    (each->GetTransform()->GetWorldPosition() - graphics::Camera::GetMainCamera()->GetTransform()->GetWorldPosition()).MagnitudeSqr() < (newPendingSelection->GetPaletteInstance()->GetTransform()->GetWorldPosition() - graphics::Camera::GetMainCamera()->GetTransform()->GetWorldPosition()).MagnitudeSqr())
                    newPendingSelection = each->GetEditableData();
            }
        }
        if (newPendingSelection == pendingSelection)
            return;
        if (pendingSelection)
            pendingSelection->GetPaletteInstance()->OnHoverLeft();
        if (newPendingSelection)
            newPendingSelection->GetPaletteInstance()->OnHover();

        pendingSelection = newPendingSelection;
    }
    void Palette::UnHoverCurrentInstance()
    {
        if (pendingSelection)
            pendingSelection->GetPaletteInstance()->OnHoverLeft();
        pendingSelection = nullptr;
    }
    bool Palette::CheckInstantiationCooltime()
    {
        if (auto currentTime = Time::GetTimeElapsedUnscaled(); currentTime - lastInstantiationTime > instantiationCooltime)
        {
            lastInstantiationTime = currentTime;
            return true;
        }
        return false;
    }
}
