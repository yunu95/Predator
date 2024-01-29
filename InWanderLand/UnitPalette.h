#pragma once
#include "Palette.h"
#include "YunutyEngine.h"
#include "SingletonClass.h"
#include "UnitData.h"

namespace application
{
    namespace editor
    {
        //class UnitData;
        class Unit_TemplateData;
        namespace palette
        {
            class UnitInstance;
            class UnitPalette : public Palette, public yunutyEngine::SingletonClass<UnitPalette>
            {
            public:
                unordered_set<UnitInstance*>& GetAllInstances();
                void ClearAllInstances();
                void AddUnitInstance(const UnitData& unitData);
                void SelectUnitTemplateData(Unit_TemplateData*);
                void UnselectUnitTemplateData();
            protected:
                virtual UnitData* PlaceInstance(Vector3d worldPosition) override;
                virtual bool ShouldSelect(IEditableData* instance) override;
                virtual void OnStartPalette() override;
                virtual void OnStandbyPalette() override;
            private:
                unordered_set<UnitInstance*> unitInstances;
                Unit_TemplateData* selectedUnitTemplateData;
            };
        }
    }
}
