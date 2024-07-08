/// 2024. 07. 08 김상준
/// Sound 편집을 위한 Editor Module

#pragma once

#include "Singleton.h"
#include "EditorModule.h"

#include "PodStructs.h"

#include <memory>

namespace application
{
    namespace editor
    {
        class Module_SoundEditor
            : public EditorModule, public Singleton<Module_SoundEditor>
        {
            friend class Singleton<Module_SoundEditor>;

        public:
            virtual ~Module_SoundEditor();

            virtual void Initialize() override;
            virtual void Update(float ts) override;
            virtual void GUIProgress() override;
            virtual void Finalize() override;

        private:
            Module_SoundEditor();

            // 한 패널에 레이아웃을 작성하는 함수
            void DrawLayout();
            // Data 리스트를 그리는 함수
            void DrawList();
        };
    }
}
