/// 2024. 03. 13 김상준
/// LightData 를 관리할 수 있도록 Debuging Mesh 등을 지원하는
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
    enum class LightType;
    class LightData;
    class Light_TemplateData;
}

namespace application
{
    namespace editor
    {
        namespace palette
        {
            class LightEditorInstance
                : public PaletteInstance
            {
            public:
                virtual ~LightEditorInstance();
                virtual void Start() override;
                void Init(const application::editor::LightData* lightData);
                void Init(const application::editor::Light_TemplateData* lightTemplateData);
                void ChangeTemplateData(const application::editor::LightData* lightData);
                void ChangeTemplateData(const application::editor::Light_TemplateData* lightTemplateData);
                void ChangeResource(const std::string& fbxName) {}
                void ChangeLight(LightType type);

                // 실제 Light Component 에 PaletteInstance 의 내용을 반영합니다.
                void ApplyLightComponent(float range, yunuGI::Color color);

                // Directional Light 는 fbxObj 를 활성화하지 않습니다.
                virtual void ShowEditorInstance() override;

            protected:
                virtual void OnHover() { PaletteInstance::OnHover(); }
                virtual void OnHoverLeft() { PaletteInstance::OnHoverLeft(); }

            private:
                LightType currentLight;
                GameObject* fbxObj;
                GameObject* lightObj;
                const application::editor::Light_TemplateData* lightTemplateData;
            };
        }
    }
}
