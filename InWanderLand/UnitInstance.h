#pragma once
#include "PaletteInstance.h"

namespace application::editor
{
    class UnitData;
    class Unit_TemplateData;
}
namespace application
{
    namespace editor
    {
        namespace palette
        {
            class UnitInstance : public PaletteInstance
            {
            public:
                virtual void Start() override;
                void Init(const application::editor::UnitData* unitData);
                void Init(const application::editor::Unit_TemplateData* unitTemplateData);
            protected:
                virtual void OnHover()
                {
                    PaletteInstance::OnHover();
                }
                virtual void OnHoverLeft()
                {
                    PaletteInstance::OnHoverLeft();
                }
                //virtual void ApplyInstance() override;
            private:
                yunutyEngine::graphics::StaticMeshRenderer* mesh{ nullptr };
                const application::editor::Unit_TemplateData* unitTemplateData;
            };
        }
    }
}
