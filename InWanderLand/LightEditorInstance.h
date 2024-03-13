/// 2024. 03. 13 김상준
/// LightData 를 관리할 수 있도록 Debuging Mesh 등을 지원하는
/// 중간 연결 클래스

#pragma once
#include "PaletteInstance.h"

#include "YunuGraphicsInterface.h"
#include <string>

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
                virtual void Start() override;
                void Init(const application::editor::LightData* lightData);
                void Init(const application::editor::Light_TemplateData* lightTemplateData);
                void ChangeTemplateData(const application::editor::LightData* lightData);
                void ChangeTemplateData(const application::editor::Light_TemplateData* lightTemplateData);
                void ChangeResource(const std::string& fbxName) {}
                void ChangeLight(LightType type);

            protected:
                virtual void OnHover() { PaletteInstance::OnHover(); }
                virtual void OnHoverLeft() { PaletteInstance::OnHoverLeft(); }

            private:
                LightType currentLight;
                const application::editor::Light_TemplateData* lightTemplateData;
            };
        }
    }
}
