#include "InWanderLand.h"
#include "Palette.h"
#include "PaletteInstance.h"
#include "SelectionBox.h"

namespace application::editor::palette
{
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
                if (selection.find(pendingSelection) == selection.end())
                {
                    ClearSelection();
                    InsertSelection(pendingSelection);
                }
                state = State::DraggingObjects;
                lastFrameBrushPos = currentBrushPos;
                UnHoverCurrentInstance();
            }
            else
            {
                state = State::DraggingSelectBox;
                dragStartPos = currentBrushPos;
                ClearSelection();
                SelectionBox::Instance().ShowSelectionBox(true);
                SelectionBox::Instance().SetCoverage(dragStartPos, currentBrushPos);
            }
            break;
        case application::editor::palette::Palette::State::Place:
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
    void Palette::OnMouseMove(Vector3d projectedWorldPos)
    {
        currentBrushPos = projectedWorldPos;
        switch (state)
        {
        case application::editor::palette::Palette::State::None:
            break;
        case application::editor::palette::Palette::State::Select:
            HoverClosestInstance();
            SelectionBox::Instance().SetCoverage(currentBrushPos - unDraggingHalfExtent, currentBrushPos + unDraggingHalfExtent);
            break;
        case application::editor::palette::Palette::State::Place:

            break;
        case application::editor::palette::Palette::State::DraggingObjects:
            for (auto each : selection)
                each->GetTransform()->SetWorldPosition(each->GetTransform()->GetWorldPosition() + currentBrushPos - lastFrameBrushPos);
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
        {
            each->GetGameObject()->GetScene()->DestroyGameObject(each->GetGameObject());
            auto& contactingInstances = SelectionBox::Instance().GetContactingInstances();
            contactingInstances.erase(each);
        }
        selection.clear();
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

    void Palette::OnSelectionContactEnter(PaletteInstance* instance)
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
    void Palette::OnSelectionContactExit(PaletteInstance* instance)
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

    void Palette::InsertSelection(PaletteInstance* instance)
    {
        if (selection.find(instance) == selection.end())
        {
            selection.insert(instance);
            instance->OnSelected();
        }

    }
    void Palette::EraseSelection(PaletteInstance* instance)
    {
        if (selection.find(instance) != selection.end())
        {
            selection.erase(instance);
            instance->OnDeselected();
        }
    }
    void Palette::ClearSelection()
    {
        for (auto each : selection)
            each->OnDeselected();
        selection.clear();
    }
    void Palette::HoverClosestInstance()
    {
        PaletteInstance* newPendingSelection{ nullptr };
        auto& contactingInstances = SelectionBox::Instance().GetContactingInstances();
        if (contactingInstances.empty())
        {
            newPendingSelection = nullptr;
        }
        else
        {
            for (auto each : contactingInstances)
            {
                if (ShouldSelect(each))
                {
                    newPendingSelection = each;
                    break;
                }
            }
            for (auto each : contactingInstances)
            {
                if (newPendingSelection == nullptr)
                    break;
                if (ShouldSelect(each) && (each->GetTransform()->GetWorldPosition() - currentBrushPos).MagnitudeSqr() < (newPendingSelection->GetTransform()->GetWorldPosition() - currentBrushPos).MagnitudeSqr())
                    newPendingSelection = each;
            }
        }
        if (newPendingSelection == pendingSelection)
            return;
        if (pendingSelection)
            pendingSelection->OnHoverLeft();
        if (newPendingSelection)
            newPendingSelection->OnHover();

        pendingSelection = newPendingSelection;
    }
    void Palette::UnHoverCurrentInstance()
    {
        if (pendingSelection)
            pendingSelection->OnHoverLeft();
        pendingSelection = nullptr;
    }
}
