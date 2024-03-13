/// 2024. 03. 12 김상준
/// CameraData 를 관리할 수 있도록 Debuging Mesh 등을 지원하는
/// 중간 연결 클래스

#pragma once
#include "PaletteInstance.h"

#include "YunuGraphicsInterface.h"
#include <string>

namespace application::editor
{
    class CameraData;
    class Camera_TemplateData;
}

namespace application
{
    namespace editor
    {
        namespace palette
        {
            class CameraEditorInstance
                : public PaletteInstance
            {
            public:
                virtual void Start() override;
                void Init(const application::editor::CameraData* cameraData);
                void Init(const application::editor::Camera_TemplateData* cameraTemplateData);
                void ChangeTemplateData(const application::editor::CameraData* cameraData) {}
                void ChangeTemplateData(const application::editor::Camera_TemplateData* cameraTemplateData) {}
                void ChangeResource(const std::string& fbxName) {}

            protected:
                virtual void OnHover() { PaletteInstance::OnHover(); }
                virtual void OnHoverLeft() { PaletteInstance::OnHoverLeft(); }

            private:
                const application::editor::Camera_TemplateData* cameraTemplateData;
            };
        }
    }
}
