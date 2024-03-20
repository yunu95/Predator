#pragma once
#include "PaletteInstance.h"
#include "YunutyEngine.h"
#include "RegionData.h"

namespace application
{
    namespace editor
    {
        namespace palette
        {
            class RegionEditorInstance : public PaletteInstance
            {
            public:
                virtual void Start() override;
                virtual void OnHover();
                virtual void OnHoverLeft();
                virtual void OnSelected();
                virtual void OnDeselected();
                void Apply(const application::editor::RegionData* regionData);

                // 지역의 가로세로 절반 길이를 반환합니다.
                void SetHalfExtent(const Vector2f& halfExtent);
                static constexpr double debugMeshYPos{ 0.02 };
            protected:
            private:
                static constexpr float regionTransparency{ 0.3f };
                Vector2f halfExtent{ 1,1 };
                yunutyEngine::graphics::StaticMeshRenderer* mesh{ nullptr };
                bool isSelected{ false };
                bool isHovering{ false };
            };
        }
    }
}
