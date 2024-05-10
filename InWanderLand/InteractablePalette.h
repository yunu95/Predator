#pragma once
#include "Palette.h"
#include "YunutyEngine.h"
#include "SingletonClass.h"

namespace application
{
    namespace editor
    {
        class InteractableData;
        class Interactable_TemplateData;
        namespace palette
        {
            class InteractableEditorInstance;
            class InteractablePalette : public Palette, public yunutyEngine::SingletonClass<InteractablePalette>
            {
            public:
                void SelectInteractableTemplateData(Interactable_TemplateData*);
                void UnselectInteractableTemplateData();

                virtual void Reset() override;
                virtual IEditableData* PlaceInstance(Vector3d worldPosition) override;
                virtual void OnMouseMove(Vector3d projectedWorldPos, Vector2d normalizedScreenPos) override;
                virtual void SetAsSelectMode(bool isSelectMode) override;
                virtual bool ShouldSelect(IEditableData* instance) override;
                virtual void OnStartPalette() override;
                virtual void OnStandbyPalette() override;

            private:
                unordered_set<InteractableEditorInstance*> interactableInstances;
                Interactable_TemplateData* selectedInteractableTemplateData{ nullptr };
            };
        }
    }
}
