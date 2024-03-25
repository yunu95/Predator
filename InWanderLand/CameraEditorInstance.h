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
                virtual ~CameraEditorInstance();
                virtual void Start() override;
                void Init(const application::editor::CameraData* cameraData);
                void Init(const application::editor::Camera_TemplateData* cameraTemplateData);
                void ChangeTemplateData(const application::editor::CameraData* cameraData) {}
                void ChangeTemplateData(const application::editor::Camera_TemplateData* cameraTemplateData) {}
                void ChangeResource(const std::string& fbxName) {}

                // 실제 Cam Component 에 PaletteInstance 의 내용을 반영합니다.
                void ApplyCamComponent(CameraData* data);

                yunutyEngine::graphics::Camera* GetCameraComponent();

                void SetAsMain();

                virtual void ShowEditorInstance() override;
                virtual void HideEditorInstance() override;

            protected:
                virtual void OnHover() { PaletteInstance::OnHover(); }
                virtual void OnHoverLeft() { PaletteInstance::OnHoverLeft(); }

            private:
                GameObject* fbxObj;
                GameObject* camObj;
                const application::editor::Camera_TemplateData* cameraTemplateData;
            };
        }
    }
}
