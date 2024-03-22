/// 2024. 02. 05 김상준
/// OrnamentData 를 관리할 수 있도록 Debuging Mesh 등을 지원하는
/// 중간 연결 클래스

#pragma once
#include "PaletteInstance.h"

#include "YunuGraphicsInterface.h"
#include <string>

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
                virtual ~OrnamentEditorInstance();
                virtual void Start() override;
                void Init(const application::editor::OrnamentData* ornamentData);
                void Init(const application::editor::Ornament_TemplateData* ornamentTemplateData);
                void ChangeTemplateData(const application::editor::OrnamentData* ornamentData);
                void ChangeTemplateData(const application::editor::Ornament_TemplateData* ornamentTemplateData);
                void ChangeResource(const std::string& fbxName);

                // meshObject 의 Transform 정보를 갱신합니다.
                void ApplyMeshTransform();

            protected:
                virtual void OnHover() { PaletteInstance::OnHover(); }
                virtual void OnHoverLeft() { PaletteInstance::OnHoverLeft(); }


            private:
                std::string currentFBX = "";
                const application::editor::Ornament_TemplateData* ornamentTemplateData;
                GameObject* meshObject;
            };
        }
    }
}
