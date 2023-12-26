#pragma once

namespace Application
{
    namespace Editor
    {
        namespace Palette
        {
            class PaletteManager
            {
            public:
                static PaletteManager* GetCurrentPalette() { return PaletteManager::currentPalette; }
            private:
                static PaletteManager* currentPalette;
            };
        }
    }
}
