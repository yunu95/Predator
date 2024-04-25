/// 2024. 04. 23 김상준
/// Particle 제작을 위한 Brush

#pragma once
#include "YunutyEngine.h"
#include "SingletonComponent.h"
#include "Particle_TemplateData.h"
#include "ParticlePalette.h"
#include "PaletteBrush.h"

namespace application
{
    namespace editor
    {
        namespace palette
        {
            class ParticleBrush : public PaletteBrush, public yunutyEngine::Component, public yunutyEngine::SingletonComponent<ParticleBrush>
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

                void ReadyBrush(Particle_TemplateData* data);

                std::unordered_map<std::string, GameObject*> brushList = std::unordered_map<std::string, GameObject*>();
                Particle_TemplateData* currentBrush = nullptr;
            };
        }
    }
}
