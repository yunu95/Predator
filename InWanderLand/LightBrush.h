/// 2024. 03. 12 김상준
/// Light 제작을 위한 Brush, 단일 브러쉬를 사용함

#pragma once
#include "YunutyEngine.h"
#include "SingletonComponent.h"
#include "Light_TemplateData.h"
#include "LightPalette.h"
#include "PaletteBrush.h"

namespace application
{
    namespace editor
    {
        namespace palette
        {
            class LightBrush : public PaletteBrush, virtual public yunutyEngine::Component, public yunutyEngine::SingletonComponent<LightBrush>
            {
            public:
                virtual void Initialize() {}
                virtual void CreateBrush() override;
                virtual bool CreateBrush(const std::string& dataKey) override;
                virtual bool ChangeBrushResource(const std::string& dataKey, const std::string& fbxName) override { return false; }
                virtual void ReadyBrush(const std::string& dataKey) override;
                virtual void Clear() override {}
                virtual void Update() override;

            private:
                virtual bool DestroyBrush(const std::string& dataKey) override { return false; };

                void ReadyBrush(Light_TemplateData* data);

                std::unordered_map<std::string, GameObject*> brushList = std::unordered_map<std::string, GameObject*>();
                Light_TemplateData* currentBrush = nullptr;
            };
        }
    }
}
