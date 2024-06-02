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

                OrnamentData* GetSingleSelectedOrnamentInstance();
                void SelectOrnamentInstance(OrnamentData* ornament);

                virtual void Reset() override;
                virtual IEditableData* PlaceInstance(Vector3d worldPosition) override;
                virtual void OnMouseMove(Vector3d projectedWorldPos, Vector2d normalizedScreenPos) override;
                virtual void SetAsSelectMode(bool isSelectMode) override;
                virtual bool ShouldSelect(IEditableData* instance) override;
                virtual void OnStartPalette() override;
                virtual void OnStandbyPalette() override;
            private:
                unordered_set<OrnamentEditorInstance*> ornamentInstances;
                Ornament_TemplateData* selectedOrnamentTemplateData{ nullptr };
            };
        }
    }
}
