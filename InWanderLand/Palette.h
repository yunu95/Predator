#pragma once
#include "YunutyEngine.h"

namespace application
{
    namespace editor
    {
        namespace palette
        {
            class SelectionBox;
            class PaletteInstance;
            /// <summary>
            /// Palette는 유닛 팔레트, 지형 팔레트, 장식물 팔레트로 특수화되는 팔레트 클래스의 베이스 클래스입니다.
            /// 이 클래스는 직접 사용자 입력을 받지 않습니다. 사용자 입력을 받는 Controller 역할은 외부에 존재하는 클래스가 수행한다고 가정합니다.
            /// \see PaletteInstance
            /// </summary>
            class Palette
            {
            public:
                // 상호 배타적인 팔레트의 현재 상태를 나타냅니다.
                enum class State
                {
                    None,
                    Select, // 객체를 선택할 수 있는 상태
                    Place, // 객체를 배치하는 상태
                    DraggingObjects, // 객체를 움직이는 상태
                    DraggingSelectBox, // 선택 박스를 조정하고 있는 상태
                };
                /// <summary>
                /// LeftClick, LeftClickRelease, OnMouseMove, OnDeletion은 적절히 호출되었을 때 다음과 같은 기능을 수행합니다.
                /// - 클릭 이벤트가 일어났을 때 가까운 인스턴스 하나를 선택합니다.
                /// - 드래그로 범위를 지정중일 때 드래깅 범위를 표시하고, 그 범위 안에 있는 인스턴스들을 선택합니다. 
                /// - 선택된 인스턴스들을 드래그하여 위치를 바꿀 수 있습니다.
                /// - 삭제 버튼이 눌렸을 때 선택된 인스턴스들을 게임오브젝트 째로 삭제합니다.
                /// - 마우스 커서가 움직일 때 해당 위치로 브러시를 이동합니다. 유닛의 경우 유닛의 실루엣이, 지형의 경우 변경사항이 적용될 지형의 실루엣이 표시됩니다.
                /// - 브러시가 존재하는 위치에 에디터 인스턴스를 배치합니다.
                /// </summary>
                virtual void OnLeftClick();
                virtual void OnLeftClickRelease();
                /// <param name="projectedWorldPos"> 마우스의 위치가 월드 좌표에 사영된 위치를 전달해 커서의 위치를 업데이트합니다.</param>
                virtual void OnMouseMove(Vector3d projectedWorldPos);
                virtual void OnDeletion();
                /// <summary>
                /// 팔레트를 객체 선택모드로 전환할지, 객체 배치 모드로 전환할지 설정합니다. isSelectMode가 참이면 선택모드로, 거짓이면 배치모드로 전환됩니다.
                /// </summary>
                virtual void SetAsSelectMode(bool isSelectMode);
                virtual bool IsSelectMode();
                /// <summary>
                /// Apply 함수는 팔레트가 관리하는 모든 에디터 객체들을 플레이타임 객체로 변환시킬때 사용됩니다.
                /// 지형은 네비게이션 메시로 구워지고, 유닛은 배치되며, 장식물은 아무것도 하지 않습니다.
                /// </summary>
                /// <returns></returns>
                virtual void ApplyAsPlaytimeObjects() = 0;
                /// <summary>
                /// 팔레트로부터 비롯된 플레이타임 객체들을 싸그리 정리합니다.
                /// </summary>
                virtual void CleanUpPlaytimeObjects() = 0;
                /// <summary>
                /// 팔레트가 선택되었을 때, 초기화 관련한 내용을 처리하는 함수입니다.
                /// </summary>
                virtual void OnStartPalette() = 0;
                /// <summary>
                /// 팔레트가 None 상태로 전환되었을 때, 현재까지 진행된 내용을 정리하는 함수입니다.
                /// 기본적으로 CleanUpData 를 호출할 것을 권장합니다.
                /// </summary>
                virtual void OnStandbyPalette() = 0;

                State GetCurrentState() { return state; }

            protected:
                /// <summary>
                /// 객체 배치를 시도할 때 호출되는 함수입니다. 해당 위치에 유닛, 지형, 장식물 등을 배치하고 배치된 객체를 반환하십시오.
                /// 부득이한 여건으로 객체를 배치할 수 없다면 Null 포인터를 반환하십시오. 
                /// </summary>
                /// <param name="worldPosition"></param>
                virtual PaletteInstance* PlaceInstance(Vector3d worldPosition) = 0;
                template<typename T> requires std::derived_from<T, yunutyEngine::Component>
                T* PlaceSoleComponent(Vector3d worldPosition)
                {
                    auto component = Scene::getCurrentScene()->AddGameObject()->AddComponent<T>();
                    component->GetTransform()->position = worldPosition;
                    return component;
                }
                bool IsClickingLeft();
                /// <summary>
                /// 마우스가 선택모드이고 선택 박스를 드래깅하지 않을때 차지하게 될 충돌크기의 halfExtents입니다.
                /// </summary>
                Vector3d unDraggingHalfExtent{ 1,1,1 };
                /// <summary>
                /// shouldSelect는 팔레트 인스턴스가 선택 콜라이더에 들어왔을 때 호출되어 해당 인스턴스를 선택할지 여부를 결정합니다.
                /// 대부분의 경우 dynamic_cast를 사용하여 해당 인스턴스가 선택 가능한 인스턴스인지 판별하는 것으로 구현될 것입니다.
                /// </summary>
                virtual bool ShouldSelect(PaletteInstance*) { return false; };
                /// <summary>
                /// OnStandbyPalette 에서 사용하기 위한, 포인터 등 데이터 정리 함수입니다.
                /// </summary>
                void CleanUpData();

                // 현재 팔레트 기능 조작의 상태
                State state{ State::Select };
                State beforeState{ State::None };
                PaletteInstance* pendingSelection;
                unordered_set<PaletteInstance*> selection;
                PaletteInstance* draggingObject = nullptr;

            private:
                // 선택 박스에 유효한 인스턴스가 접촉되기 시작했을 때, 혹은 접촉이 끝났을 때 호출됩니다.
                void OnSelectionContactEnter(PaletteInstance* instance);
                void OnSelectionContactExit(PaletteInstance* instance);
                // 선택 목록에 인스턴스를 추가 / 제거
                void InsertSelection(PaletteInstance* instance);
                void EraseSelection(PaletteInstance* instance);
                void ClearSelection();
                // 선택박스와 접촉중인 객체 중 가장 마우스포인터와 가까운 인스턴스의 OnHover 함수를 호출합니다.
                // 접촉중인 객체가 없다면 현재 마킹된 인스턴스의 OnHoverLeft 함수를 호출합니다.
                void HoverClosestInstance();
                // 현재 호버링중인 객체가 있다면 OnHoverLeft함수를 호출합니다. 
                void UnHoverCurrentInstance();
                
                bool isClickingLeft{ false };
                Vector3d dragStartPos;
                Vector3d currentBrushPos;
                Vector3d lastFrameBrushPos;
                friend SelectionBox;
            };
        }
    }
}
