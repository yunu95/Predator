#pragma once
#include "Palette.h"
#include "YunutyEngine.h"
#include "SingletonClass.h"
#include "OrnamentData.h"

namespace application
{
    namespace editor
    {
		class Ornament_TemplateData;
        namespace palette
        {
            class OrnamentEditorInstance;
            class OrnamentPalette : public Palette, public yunutyEngine::SingletonClass<OrnamentPalette>
            {
            public:
                void SelectOrnamentTemplateData(Ornament_TemplateData*);
                void UnselectOrnamentTemplateData();

            protected:
                virtual void Reset() override;
                virtual IEditableData* PlaceInstance(Vector3d worldPosition) override;
                virtual bool ShouldSelect(IEditableData* instance) override;
                virtual void OnStartPalette() override;
                virtual void OnStandbyPalette() override;
                virtual void CleanUpData() override;
            private:
                unordered_set<OrnamentEditorInstance*> ornamentInstances;
                Ornament_TemplateData* selectedOrnamentTemplateData{ nullptr };
            };
        }
    }
}
