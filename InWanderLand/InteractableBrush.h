/// 2024. 05. 09 김상준
/// Interactable Palette 에서 Interactable 을 생성할 때 가이드 역할을 할 Brush 입니다.

#pragma once
#include "YunutyEngine.h"
#include "SingletonComponent.h"

#include "Interactable_TemplateData.h"
#include "PaletteBrush.h"

#include <unordered_map>

namespace application
{
    namespace editor
    {
        namespace palette
        {
            class InteractableBrush : public PaletteBrush, virtual public yunutyEngine::Component, public yunutyEngine::SingletonComponent<InteractableBrush>
            {
            public:
                virtual void Initialize() {}
                virtual void CreateBrush() override;
                virtual bool CreateBrush(const std::string& dataKey) override;
                virtual bool ChangeBrushResource(const std::string& dataKey, const std::string& fbxName) override;
                virtual void ReadyBrush(const std::string& dataKey) override;
                virtual void Clear() override;
                virtual void Update() override;

            private:
                virtual bool DestroyBrush(const std::string& dataKey) override;

                void ReadyBrush(Interactable_TemplateData* data);

                std::unordered_map<std::string, GameObject*> brushList = std::unordered_map<std::string, GameObject*>();
                Interactable_TemplateData* currentBrush = nullptr;
            };
        }
    }
}
