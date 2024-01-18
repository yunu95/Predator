#ifdef EDITOR
#include "InWanderLand.h"
#include "Panel_Palette.h"
#include "Application.h"

#include "YunutyEngine.h"

#include "imgui.h"

namespace application
{
    namespace editor
    {
        PalettePanel::PalettePanel()
        {

        }

        PalettePanel::~PalettePanel()
        {

        }

        void PalettePanel::Initialize()
        {

        }

        void PalettePanel::Update(float ts)
        {

        }

        void PalettePanel::GUIProgress()
        {
            ImGui::Begin("Palette");

            /// ImGui 관련 내부 변수 업데이트
            isMouseOver = ImGui::IsWindowHovered();
			isFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

            /// 실제 패널에 그리는 영역

            /// 아래 부분은 Palette가 오로지 장식물 배치에만 사용된다고 가정하고 작성한 코드입니다.
            /// 메시 타입을 결정한 후 
            // 메시 타입을 설정합니다.

            ImGui::End();
        }

        void PalettePanel::Finalize()
        {

        }
    }
}
#endif
