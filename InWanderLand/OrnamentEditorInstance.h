/// 2024. 02. 05 김상준
/// OrnamentData 를 관리할 수 있도록 Debuging Mesh 등을 지원하는
/// 중간 연결 클래스

#pragma once
#include "PaletteInstance.h"

#include "YunuGraphicsInterface.h"

namespace application::editor
{
    class OrnamentData;
    class Ornament_TemplateData;
}

namespace application
{
    namespace editor
    {
        namespace palette
        {
            class OrnamentEditorInstance
                : public PaletteInstance
            {
            public:
                virtual void Start() override;
                void Init(const application::editor::OrnamentData* ornamentData);
                void Init(const application::editor::Ornament_TemplateData* ornamentTemplateData);
                void ResizeMesh(yunuGI::Vector3 scale);

            protected:
                virtual void OnHover() { PaletteInstance::OnHover(); }
                virtual void OnHoverLeft() { PaletteInstance::OnHoverLeft(); }

            private:
                yunutyEngine::graphics::StaticMeshRenderer* mesh{ nullptr };
                const application::editor::Ornament_TemplateData* ornamentTemplateData;
            };
        }
    }
}
