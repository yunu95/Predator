/// 2024. 04. 23 김상준
/// ParticleData 를 관리할 수 있도록 Debuging Mesh 등을 지원하는
/// 중간 연결 클래스

#pragma once
#include "PaletteInstance.h"

#include "YunuGraphicsInterface.h"
#include <string>

namespace yunutyEngine
{
    class GameObject;
}

namespace application::editor
{
    enum class ParticleType;
    class ParticleData;
    class Particle_TemplateData;
}

namespace application
{
    namespace editor
    {
        namespace palette
        {
            class ParticleEditorInstance
                : public PaletteInstance
            {
            public:
                virtual ~ParticleEditorInstance();
                virtual void Start() override;
                void Init(const application::editor::ParticleData* particleData);
                void Init(const application::editor::Particle_TemplateData* particleTemplateData);
                void ChangeTemplateData(const application::editor::ParticleData* particleData) {}
                void ChangeTemplateData(const application::editor::Particle_TemplateData* particlTemplateData) {}

                // 실제 Particle Component 에 ParticleData 의 내용을 반영합니다.
                void ApplyParticleComponent(ParticleData* data);

                // Directional Particle 는 fbxObj 를 활성화하지 않습니다.
                virtual void ShowEditorInstance() override;
                virtual void HideEditorInstance() override;

                void ChangeGuideInstance();

                void ShowParticleObject();
                void HideParticleObject();

                GameObject* GetParticleObject();

            protected:
                virtual void OnHover() { PaletteInstance::OnHover(); }
                virtual void OnHoverLeft() { PaletteInstance::OnHoverLeft(); }

            private:
                bool isGuide = false;
                GameObject* fbxObj;
                GameObject* particleObj;
                const application::editor::Particle_TemplateData* particleTemplateData;
            };
        }
    }
}
