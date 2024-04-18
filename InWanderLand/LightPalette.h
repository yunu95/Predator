/// 2024. 03. 13 김상준
/// Light 제작용 Palette

#pragma once
#include "Palette.h"
#include "YunutyEngine.h"
#include "SingletonClass.h"
#include "LightData.h"

namespace application
{
    namespace editor
    {
        class Light_TemplateData;
        namespace palette
        {
            class LightEditorInstance;
            class LightPalette : public Palette, public yunutyEngine::SingletonClass<LightPalette>
            {
            public:
                LightData* GetSingleSelectedLight();
                void SelectLight(LightData* light);

                void SelectLightTemplateData(Light_TemplateData*);
                void UnselectLightTemplateData();

                virtual void Initialize();
                virtual void Reset() override;
                virtual IEditableData* PlaceInstance(Vector3d worldPosition) override;
                virtual void OnMouseMove(Vector3d projectedWorldPos, Vector2d normalizedScreenPos) override;
                virtual void SetAsSelectMode(bool isSelectMode) override;
                virtual bool ShouldSelect(IEditableData* instance) override;
                virtual void OnStartPalette() override;
                virtual void OnStandbyPalette() override;

            private:
                unordered_set<LightEditorInstance*> lightInstances;
                Light_TemplateData* selectedLightTemplateData{ nullptr };
            };
        }
    }
}
