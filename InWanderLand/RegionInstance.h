#pragma once
#ifdef EDITOR
#include "PaletteInstance.h"
#include "YunutyEngine.h"

namespace application
{
    namespace editor
    {
        namespace palette
        {
            class RegionInstance : public PaletteInstance
            {
            public:
                virtual void Start() override;
                virtual void OnHover();
                virtual void OnHoverLeft();
                virtual void OnSelected();
                virtual void OnDeselected();

                // 지역의 가로세로 절반 길이를 반환합니다.
                void SetHalfExtent(const Vector2f& halfExtent);
            protected:
                virtual void ApplyInstance() {};
            private:
                Vector2f halfExtent{ 1,1 };
                yunutyEngine::graphics::StaticMeshRenderer* mesh{ nullptr };
                bool isSelected{ false };
                bool isHovering{ false };
            };
        }
    }
}
#endif
