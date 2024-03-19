#pragma once
#include "YunutyEngine.h"

namespace application
{
    namespace editor
    {
        class IEditableData;
        namespace palette
        {
            /// <summary>
            /// PaletteInstance는 팔레트에서 맵에 배치할 수 있는 객체의 최소 단위입니다. 이 객체의 상태는 파일로 저장되어야 하며 Serialization, Deserialization의 대상이 됩니다.
            /// 
            /// <attention>
            /// 
            /// - PaletteInstance는 인게임 진행에 따라 상태가 변하지 않아야 합니다.
            /// 예를 들어, 게임을 시작하고 유닛을 살해하거나, 보스전을 진행하는 등 게임의 상태가 계속 바뀌더라도 PaletteInstance의 상태는 변하지 않아야 합니다.
            /// PaletteInstance의 값들을 바꿀 수 있는 함수는 모두 에디터에서만 호출되어야 합니다.
            /// 
            /// - PaletteInstance는 에디터가 활성화된 상태와 에디터가 비활성화된 상태에 따라 최적화된 동작을 수행해야 합니다.
            /// 예를 들어, 에디터가 활성화된 상태에서는 오브젝트 선택을 위해 PhysX Kinematic Rigidody를 사용해야 하고, 디버그 메시도 표시해야 하지만
            /// 에디터가 비활성화된 상태에서는 RigidBody나 디버그 메시에 들어가는 자원을 사용하지 않아야 합니다.
            /// </attention>
            /// </summary>
            class PaletteInstance : public yunutyEngine::Component
            {
            public:
                virtual void Start() override;
                /// <summary>
                /// OnSelected는 객체가 선택되었을 때 호출됩니다. 객체가 선택되었음을 명확히 알 수 있도록 그래픽스 효과를 구현하십시오. 
                /// OnHover는 팔레트 선택 모드에서 마우스 커서가 해당 객체에 올라갔을 때 호출됩니다. OnSelected와 유사하지만 구분은 가능한 효과를 구현하십시오.
                /// OnDeselcted는 객체가 선택 해제되었을 때, 혹은 호버링 대상에서 벗어났을 때 호출됩니다.
                /// 이 세가지 함수들은 중복되어 호출될 수 있습니다. 멱등성을 고려해 구현하십시오. 
                /// 예를 들어, OnSelected 함수가 두번 연속 호출된 경우와 한번 호출된 경우의 결과는 서로 같아야 합니다.
                /// </summary>
                void AdjustPickingCollider(const Vector3f& boundingMin, const Vector3f& boundingMax);
                virtual void OnHover();
                virtual void OnHoverLeft();
                virtual void OnSelected();
                virtual void OnDeselected();
                /// <summary>
                /// 현재 존재하는 모든 PaletteInstance들을 에디터 요소에서 인게임 요소로 활성화시킵니다.
                /// 지형의 경우 네비게이션 메시를 구워 인게임에서 오갈 수 있는 지형으로 만듭니다.
                /// 유닛의 경우 인게임 유닛을 생성합니다.
                /// </summary>
                static void ApplyInstances();
                /// <summary>
                /// 현재 게임 요소로서 활성화된 모든 PaletteInstance들을 에디터에서 조작 가능한 상태로 되돌립니다.
                /// 기본 동작은 피킹을 위해 쓰이는 충돌체들을 다시 활성화시킵니다.
                /// </summary>
                static void ResetInstances();
                IEditableData* GetEditableData();
                void SetEditableData(IEditableData* data);

            protected:
                //virtual void ApplyInstance();
                virtual void EnablePickingCollider();
                virtual void DisablePickingCollider();
                float selectCircleRadius{ 1.25 };
                static constexpr float transparency{ 0.3f };
                yunutyEngine::physics::BoxCollider* pickingCollider{ nullptr };
                yunutyEngine::graphics::StaticMeshRenderer* selectCircle{nullptr};
            private:
                void InitPickingCollider();
                IEditableData* editableData{ nullptr };
                yunutyEngine::physics::RigidBody* rigidBody{ nullptr };
                bool isSelected{ false };
                bool isHovering{ false };
                friend IEditableData;
            };
        }
    }
}
