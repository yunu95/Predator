#pragma once
#include "Palette.h"
#include "YunutyEngine.h"
#include "SingletonClass.h"

namespace application
{
    namespace editor
    {
        class UnitData;
        class Unit_TemplateData;
        namespace palette
        {
            class UnitEditorInstance;
            class UnitPalette : public Palette, public yunutyEngine::SingletonClass<UnitPalette>
            {
            public:
                void SelectUnitTemplateData(Unit_TemplateData*);
                void UnselectUnitTemplateData();

                virtual void Reset() override;
                virtual IEditableData* PlaceInstance(Vector3d worldPosition) override;
                virtual void OnMouseMove(Vector3d projectedWorldPos, Vector2d normalizedScreenPos) override;
                virtual void SetAsSelectMode(bool isSelectMode) override;
                virtual bool ShouldSelect(IEditableData* instance) override;
                virtual void OnStartPalette() override;
                virtual void OnStandbyPalette() override;
                virtual void Delete(IEditableData* data) override;

            private:
                unordered_set<UnitEditorInstance*> unitInstances;
                Unit_TemplateData* selectedUnitTemplateData{ nullptr };
            };
        }
    }
}
