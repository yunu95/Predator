/// 2024. 02. 05 김상준
/// OrnamentData 를 관리할 수 있도록 Debuging Mesh 등을 지원하는
/// 중간 연결 클래스

#pragma once
#include "PaletteInstance.h"

#include "YunuGraphicsInterface.h"
#include "StaticInstanceRegistry.h"
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
                : public PaletteInstance, public StaticInstanceRegistry<OrnamentEditorInstance>
            {
            public:
                virtual void Start() override;
                virtual void Update() override;
                void Init(const application::editor::OrnamentData* ornamentData);
                void Init(const application::editor::Ornament_TemplateData* ornamentTemplateData);
                void ChangeTemplateData(const application::editor::OrnamentData* ornamentData);
                void ChangeTemplateData(const application::editor::Ornament_TemplateData* ornamentTemplateData);
                void ChangeResource(const std::string& fbxName);
                virtual void ShowEditorInstance() override;
                virtual void HideEditorInstance() override;
                const application::editor::OrnamentData* GetOrnamentData();

                void ChangeGuideInstance();
                void SetVisibleWithFading(bool visible);
                bool fadeVisible = true;
                coroutine::Coroutine Fade(bool& visible);
                std::weak_ptr<coroutine::Coroutine> fadeCoroutine;

            protected:
                virtual void OnHover() { PaletteInstance::OnHover(); }
                virtual void OnHoverLeft() { PaletteInstance::OnHoverLeft(); }

            private:
                bool actionActiveFlag = true;
                bool isGuide = false;
                std::string currentFBX = "";
                const application::editor::OrnamentData* ornamentData{};
                const application::editor::Ornament_TemplateData* ornamentTemplateData;
            };
        }
    }
}
