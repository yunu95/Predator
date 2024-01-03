#include "PaletteManager.h"
#include "SelectionBox.h"

namespace application::editor::palette
{
    PaletteManager* PaletteManager::currentPalette = nullptr;

    void PaletteManager::OnLeftClick()
    {
        dragStartPos = currentBrushPos;
        /// 호버링 되는 상태의 객체가 있다면 객체를 단 하나만 선택합니다. 선택된 객체가 없다면 눈치를 보다가 드래깅을 시작합니다.
    }
    void PaletteManager::OnLeftClickRelease()
    {
        /// 드래깅 선택을 진행중인 상태라면 드래깅 박스를 비활성화합니다.
        if ((dragStartPos - currentBrushPos).MagnitudeSqr() < dragThreshold * dragThreshold)
        {
        }
        else
        {
        }
    }
    void PaletteManager::OnMouseMove(Vector3d projectedWorldPos)
    {
        currentBrushPos = projectedWorldPos;
        if (isDraggingForSelection())
        {
            /// 셀렉션 박스를 활성화시키고 리사이징합니다.
        }
        /// 개별 인스턴스 드래깅 상태라면 드래깅되는 객체의 위치를 옮깁니다.
    }
    void PaletteManager::OnDeletion()
    {

    }
    bool PaletteManager::isDraggingForSelection()
    {
        return Input::isKeyDown(KeyCode::MouseLeftClick) && (dragStartPos - currentBrushPos).MagnitudeSqr() > dragThreshold * dragThreshold;
    }
}
