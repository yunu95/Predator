/// 2024. 03. 12 김상준
/// Camera 제작을 위한 Brush, 단일 브러쉬를 사용함

#pragma once
#include "YunutyEngine.h"
#include "SingletonComponent.h"
#include "Camera_TemplateData.h"
#include "CameraPalette.h"
#include "PaletteBrush.h"

namespace application
{
    namespace editor
    {
        namespace palette
        {
            class CameraBrush : public PaletteBrush, virtual public yunutyEngine::Component, public yunutyEngine::SingletonComponent<CameraBrush>
            {
            public:
                virtual void Initialize() {}
                virtual void CreateBrush() override;
                virtual bool CreateBrush(const std::string& dataKey) override { return false; }
                virtual bool ChangeBrushResource(const std::string& dataKey, const std::string& fbxName) override { return false; }
                virtual void ReadyBrush(const std::string& dataKey) override;
                virtual void Clear() override {}
                
            private:
                virtual bool DestroyBrush(const std::string& dataKey) override { return false; };

                void ReadyBrush(Camera_TemplateData* data);

                GameObject* brush = nullptr;
            };
        }
    }
}
