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
            class UnitEditorInstance : public PaletteInstance
            {
            public:
                virtual void Start() override;
                void Init(const application::editor::UnitData* unitData);
                void Init(const application::editor::Unit_TemplateData* unitTemplateData);
                void ChangeTemplateData(const application::editor::UnitData* unitData);
                void ChangeTemplateData(const application::editor::Unit_TemplateData* unitTemplateData);
                void ChangeResource(const std::string& fbxName);

            protected:
                virtual void OnHover()
                {
                    PaletteInstance::OnHover();
                }
                virtual void OnHoverLeft()
                {
                    PaletteInstance::OnHoverLeft();
                }
            private:
                std::string currentFBX = "";
                const application::editor::Unit_TemplateData* unitTemplateData;
            };
        }
    }
}
