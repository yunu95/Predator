#include "PaletteManager.h"
#include "SelectionBox.h"

#include "PaletteList.h"

namespace application
{
    namespace editor
    {
        namespace palette
        {
            void PaletteManager::Initialize()
            {
                paletteList[(int)Palette_List::Terrain] = &TerrainPalette::SingleInstance();
                paletteList[(int)Palette_List::Unit] = &UnitPalette::SingleInstance();
                paletteList[(int)Palette_List::Ornament] = &OrnamentPalette::SingleInstance();
                paletteList[(int)Palette_List::Region] = &RegionPalette::SingleInstance();
                paletteList[(int)Palette_List::Wave] = &WavePalette::SingleInstance();
                paletteList[(int)Palette_List::Cam] = &CameraPalette::SingleInstance();
                paletteList[(int)Palette_List::Light] = &LightPalette::SingleInstance();
                paletteList[(int)Palette_List::Particle] = &ParticlePalette::SingleInstance();

                for (auto& each : paletteList)
                {
                    each->Initialize();
                }

                SetCurrentPalette(paletteList[(int)Palette_List::Terrain]);
            }

            void PaletteManager::Clear()
            {
                SelectionBox::Instance().contactingInstances.clear();
                for (auto& each : paletteList)
                {
                    if (each)
                    {
                        each->Reset();
                    }
                }
            }

            void PaletteManager::SetCurrentPalette(Palette* palette)
            {
                if (currentPalette)
                {
                    currentPalette->OnStandbyPalette();
                }

                currentPalette = palette;

                if (currentPalette)
                {
                    currentPalette->OnStartPalette();
                }
            }

            Palette_List PaletteManager::GetCurrentPaletteType()
            {
                if (currentPalette == paletteList[(int)Palette_List::Terrain])
                {
                    return Palette_List::Terrain;
                }
                else if (currentPalette == paletteList[(int)Palette_List::Unit])
                {
                    return Palette_List::Unit;
                }
                else if (currentPalette == paletteList[(int)Palette_List::Ornament])
                {
                    return Palette_List::Ornament;
                }
                else if (currentPalette == paletteList[(int)Palette_List::Region])
                {
                    return Palette_List::Region;
                }
                else if (currentPalette == paletteList[(int)Palette_List::Wave])
                {
                    return Palette_List::Wave;
                }
                else if (currentPalette == paletteList[(int)Palette_List::Cam])
                {
                    return Palette_List::Cam;
                }
                else if (currentPalette == paletteList[(int)Palette_List::Light])
                {
                    return Palette_List::Light;
                }
                else if (currentPalette == paletteList[(int)Palette_List::Particle])
                {
                    return Palette_List::Particle;
                }

                return Palette_List::None;
            }

            PaletteManager::PaletteManager()
                : paletteList(), currentPalette(nullptr)
            {
                paletteList.resize((int)Palette_List::Size);
            }
        }
    }
}
