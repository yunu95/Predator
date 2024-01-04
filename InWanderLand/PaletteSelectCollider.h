#pragma once
#include "YunutyEngine.h"
#include "SingletonComponent.h"

namespace application
{
    namespace editor
    {
        namespace palette
        {
            /// <summary>
            /// 브러시는 월드 좌표에 사영된 마우스 좌표에 그래픽스 객체를 배치해 어느 영역을 어떻게 수정하는 것인지 사용자에게 알려줍니다.
            /// 또한 PhysX 트리거로 PaletteInstance들과 충돌하여 인스턴스 선택을 구현합니다.
            /// </summary>
            class PaletteBrush : public SingletonComponent<PaletteBrush>
            {
            public:
            protected:
            private:
            };
        }
    }
}
