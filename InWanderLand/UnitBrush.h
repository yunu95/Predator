/// 2024. 02. 08 김상준
/// Unit Palette 에서 Unit 을 생성할 때 가이드 역할을 할 Brush 입니다.

#pragma once
#include "YunutyEngine.h"
#include "SingletonComponent.h"

#include "Unit_TemplateData.h"
#include "PaletteBrush.h"

#include <unordered_map>

namespace application
{
    namespace editor
    {
        namespace palette
        {
            class UnitBrush : public PaletteBrush, public yunutyEngine::Component, public yunutyEngine::SingletonComponent<UnitBrush>
            {
            public:
                virtual void CreateBrush() override;
                virtual bool CreateBrush(const std::string& dataKey) override;
                virtual bool ChangeBrushResource(const std::string& dataKey, const std::string& fbxName) override;
                virtual void ReadyBrush(const std::string& dataKey) override;
                virtual void Clear() override;
                virtual void Update() override;

            private:
                virtual bool DestroyBrush(const std::string& dataKey) override;

                void ReadyBrush(Unit_TemplateData* data);

                std::unordered_map<std::string, GameObject*> brushList = std::unordered_map<std::string, GameObject*>();
                Unit_TemplateData* currentBrush = nullptr;
            };
        }
    }
}
