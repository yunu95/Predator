/// 2024. 01. 18 김상준
/// Palette 를 관리하는 Manager 클래스

#pragma once

#include "Singleton.h"

#include <vector>

namespace application
{
    namespace editor
    {
        namespace palette
        {
            class Palette;
        }
    }
}

namespace application
{
    namespace editor
    {
        namespace palette
        {
            enum class Palette_List
            {
                Terrain = 0,
                Unit,
                Ornament,
                Region,
                Wave,
                Cam,
                Light,
                Particle,
                Interactable,

                /// Size를 자동으로 넣기 위해 사용하는 enum
                /// 첫 enum 값이 0 이고, 모든 간격이 1일 때에 가능함
                Size,
                /// 아무것도 선택되지 않았을 경우를 위한 enum
                None
            };

            class PaletteManager
                : public Singleton<PaletteManager>
            {
                friend class Singleton<PaletteManager>;

            public:
                void Initialize();
                void Clear();

                /// <summary>
                /// 현재 활성화된 팔레트를 지정하거나 가져올 수 있습니다.
                /// </summary>
                /// <returns></returns>
                void SetCurrentPalette(Palette* palette);
                Palette* GetCurrentPalette() { return currentPalette; }
                Palette_List GetCurrentPaletteType();
            private:
                PaletteManager();

                std::vector<Palette*> paletteList;
                Palette* currentPalette;
            };
        }
    }
}
