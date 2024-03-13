/// 2024. 03. 07 김상준
/// Brush 관리를 위한 Manager 클래스

#pragma once

#include "Singleton.h"

#include "PaletteBrush.h"

#include <vector>

namespace application
{
    namespace editor
    {
        namespace palette
        {
            class PaletteBrushManager
                : public Singleton<PaletteBrushManager>
            {
                friend class Singleton<PaletteBrushManager>;

            public:
                void Initialize();
                void Clear();
                void MakeBrush();

            private:
                PaletteBrushManager();

                std::vector<PaletteBrush*> brushList;
            };
        }
    }
}
