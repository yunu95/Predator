#pragma once
#ifdef EDITOR
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
                virtual void Start()override;
            protected:
                virtual void OnHover()
                {
                    PaletteInstance::OnHover();
                }
                virtual void OnHoverLeft()
                {
                    PaletteInstance::OnHoverLeft();
                }
                virtual void ApplyInstance() override;
            private:
                yunutyEngine::graphics::StaticMeshRenderer* mesh{ nullptr };
            };
        }
    }
}
#endif
