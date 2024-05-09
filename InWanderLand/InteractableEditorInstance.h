#pragma once
#include "PaletteInstance.h"

namespace application::editor
{
    class InteractableData;
    class Interactable_TemplateData;
}
namespace application
{
    namespace editor
    {
        namespace palette
        {
            class InteractableEditorInstance : public PaletteInstance
            {
            public:
                virtual void Start() override;
                void Init(const application::editor::InteractableData* interactableData);
                void Init(const application::editor::Interactable_TemplateData* interactableTemplateData);
                void ChangeTemplateData(const application::editor::InteractableData* interactableData);
                void ChangeTemplateData(const application::editor::Interactable_TemplateData* interactableTemplateData);
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
                const application::editor::Interactable_TemplateData* interactableTemplateData;
            };
        }
    }
}
