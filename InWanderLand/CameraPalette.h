/// 2024. 03. 12 김상준
/// Camera 제작용 Palette

#pragma once
#include "Palette.h"
#include "YunutyEngine.h"
#include "SingletonClass.h"
#include "CameraData.h"

namespace application
{
    namespace editor
    {
        //class CameraData;
        class Camera_TemplateData;
        namespace palette
        {
            class CameraEditorInstance;
            class CameraPalette : public Palette, public yunutyEngine::SingletonClass<CameraPalette>
            {
            public:
                virtual void Initialize();
                virtual void Reset() override;
                virtual CameraData* PlaceInstance(Vector3d worldPosition) override;
                virtual void OnMouseMove(Vector3d projectedWorldPos, Vector2d normalizedScreenPos) override;
                virtual void SetAsSelectMode(bool isSelectMode) override;
                virtual bool ShouldSelect(IEditableData* instance) override;
                virtual void OnStartPalette() override;
                virtual void OnStandbyPalette() override;
                virtual void CleanUpData() override;

            private:
                unordered_set<CameraEditorInstance*> cameraInstances;
            };
        }
    }
}
