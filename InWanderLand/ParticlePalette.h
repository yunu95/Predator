/// 2024. 04. 23 김상준
/// Particle 제작용 Palette

#pragma once
#include "Palette.h"
#include "YunutyEngine.h"
#include "SingletonClass.h"
#include "ParticleToolData.h"

namespace application
{
    namespace editor
    {
        class ParticleData;
    }
}

namespace application
{
    namespace editor
    {
        class Particle_TemplateData;
        namespace palette
        {
            class ParticleEditorInstance;
            class ParticlePalette : public Palette, public yunutyEngine::SingletonClass<ParticlePalette>
            {
            public:
                ParticleData* GetSingleSelectedParticle();
                void SelectParticle(ParticleData* particle);

                void SetParticleData(const particle::ParticleToolData& data);
                void InitParticleData();

                virtual void Initialize();
                virtual void Reset() override;
                virtual IEditableData* PlaceInstance(Vector3d worldPosition) override;
                virtual void OnMouseMove(Vector3d projectedWorldPos, Vector2d normalizedScreenPos) override;
                virtual void SetAsSelectMode(bool isSelectMode) override;
                virtual bool ShouldSelect(IEditableData* instance) override;
                virtual void OnStartPalette() override;
                virtual void OnStandbyPalette() override;

            private:
                unordered_set<ParticleEditorInstance*> particleInstances;
                particle::ParticleToolData mold = particle::ParticleToolData();
            };
        }
    }
}
