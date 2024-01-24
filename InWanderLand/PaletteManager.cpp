#include "PaletteManager.h"

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
				paletteList[(int)Palette_List::Doodad] = &DoodadPalette::SingleInstance();
				paletteList[(int)Palette_List::Region] = &RegionPalette::SingleInstance();

				SetCurrentPalette(paletteList[(int)Palette_List::Terrain]);
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

			PaletteManager::PaletteManager()
				: paletteList(), currentPalette(nullptr)
			{
				paletteList.resize((int)Palette_List::Size);
			}
		}
	}
}
