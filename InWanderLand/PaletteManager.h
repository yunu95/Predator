#pragma once
#include "YunutyEngine.h"

namespace application
{
    namespace editor
    {
        namespace palette
        {
            class PaletteInstance;
            /// <summary>
            /// PaletteManager는 유닛 팔레트, 지형 팔레트, 장식물 팔레트로 특수화되는 팔레트 관리 클래스의 베이스 클래스입니다.
            /// 이 클래스는 직접 사용자 입력을 받지 않습니다. 사용자 입력을 받는 Controller 역할은 외부에 존재하는 클래스가 수행한다고 가정합니다.
            /// \see PaletteInstance
            /// </summary>
            class PaletteManager
            {
            public:
                /// <summary>
                /// 현재 활성화된 팔레트를 지정하거나 가져올 수 있습니다.
                /// </summary>
                /// <returns></returns>
                static void SetCurrentPalette(PaletteManager* palette) { PaletteManager::currentPalette = palette; }
                static PaletteManager* GetCurrentPalette() { return PaletteManager::currentPalette; }
                /// <summary>
                /// LeftClick, LeftClickRelease, OnMouseMove, OnDeletion은 적절히 호출되었을 때 다음과 같은 기능을 수행합니다.
                /// - 클릭 이벤트가 일어났을 때 가까운 인스턴스 하나를 선택합니다.
                /// - 드래그로 범위를 지정중일 때 드래깅 범위를 표시하고, 그 범위 안에 있는 인스턴스들을 선택합니다. 
                /// - 선택된 인스턴스들을 드래그하여 위치를 바꿀 수 있습니다.
                /// - 삭제 버튼이 눌렸을 때 선택된 인스턴스들을 게임오브젝트 째로 삭제합니다.
                /// - 마우스 커서가 움직일 때 해당 위치로 브러시를 이동합니다. 유닛의 경우 유닛의 실루엣이, 지형의 경우 변경사항이 적용될 지형의 실루엣이 표시됩니다.
                /// - 브러시가 존재하는 위치에 에디터 인스턴스를 배치합니다.
                /// </summary>
                void OnLeftClick();
                void OnLeftClickRelease();
                /// <param name="projectedWorldPos"> 마우스의 위치가 월드 좌표에 사영된 위치를 전달해 커서의 위치를 업데이트합니다.</param>
                void OnMouseMove(Vector3d projectedWorldPos);
                void OnDeletion();
            protected:
                /// <summary>
                /// 하나의 PaletteManager로부터 선택된 인스턴스들입니다. 유닛 팔레트의 경우 유닛, 지형 팔레트의 경우 지형 한 칸에 대응합니다.
                /// </summary>
                vector<PaletteInstance*> selectedInstances;
                /// <summary>
                /// 객체 배치를 시도할 때 호출되는 함수입니다. 해당 위치에 유닛, 지형, 장식물 등을 배치하고 배치된 객체를 반환하십시오.
                /// 부득이한 여건으로 객체를 배치할 수 없다면 Null 포인터를 반환하십시오. 
                /// </summary>
                /// <param name="worldPosition"></param>
                virtual PaletteInstance* PlaceInstance(Vector3d worldPosition) = 0;
                /// <summary>
                /// 현재 팔레트 모드가 선택모드인지 아닌지 반환합니다. 선택모드일 때는 브러시가 표시되지 않으며, 마우스 좌클릭이 객체 배치가 아닌 객체 선택에 사용됩니다.
                /// </summary>
                virtual bool isSelectMode() = 0;
                /// <summary>
                /// shouldSelect는 팔레트 인스턴스가 선택 콜라이더에 들어왔을 때 호출되어 해당 인스턴스를 선택할지 여부를 결정합니다.
                /// 대부분의 경우 dynamic_cast를 사용하여 해당 인스턴스가 선택 가능한 인스턴스인지 판별하는 것으로 구현될 것입니다.
                /// </summary>
                virtual bool shouldSelect(PaletteInstance*) { return false; };
                /// <summary>
                /// 브러시 오브젝트는 팔레트 모드가 선택모드가 아닐 때 마우스 커서가 월드 스페이스에 사영된 위치로 이동합니다.
                /// 팔레트 모드가 선택모드일 때는 게임오브젝트가 비활성화되며 브러시 오브젝트가 표시되지 않습니다.
                /// </summary>
                GameObject* brush;
            private:
                /// <summary>
                /// 사용자가 지금 여러 객체들을 선택하기 위해 드래깅 중인가? 를 판별합니다.
                /// </summary>
                bool isDraggingForSelection();
                static PaletteManager* currentPalette;
                PaletteInstance* draggingObject = nullptr;
                Vector3d dragStartPos;
                Vector3d currentBrushPos;
                float draggingTime = 0;
                static constexpr float dragThreshold = 0.5f;
            };
        }
    }
}
