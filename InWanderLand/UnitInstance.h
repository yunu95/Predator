#pragma once
#include "PaletteInstance.h"

namespace application
{
    namespace editor
    {
        namespace palette
        {
            class UnitInstance : public PaletteInstance
            {
            public:
#ifdef EDITOR
                virtual void Start()override;
            protected:
                virtual void ApplyInstance() override;
                virtual void OnHover()
                {
                    PaletteInstance::OnHover();
                }
                virtual void OnHoverLeft()
                {
                    PaletteInstance::OnHoverLeft();
                }
#endif
            private:
                yunutyEngine::graphics::StaticMeshRenderer* mesh{ nullptr };
            };
        }
    }
}
