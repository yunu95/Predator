#pragma once
#include "YunutyEngine.h"
#include "SingletonComponent.h"

#include "PaletteBrush.h"
#include "Ornament_TemplateData.h"

#include <string>
#include <unordered_map>

namespace application
{
    namespace editor
    {
        namespace palette
        {
            class OrnamentBrush : public PaletteBrush, public yunutyEngine::Component, public yunutyEngine::SingletonComponent<OrnamentBrush>
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

                void ReadyBrush(Ornament_TemplateData* data);

                std::unordered_map<std::string, GameObject*> brushList = std::unordered_map<std::string, GameObject*>();
                Ornament_TemplateData* currentBrush = nullptr;
            };
        }
    }
}

