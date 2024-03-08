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
				paletteList[(int)Palette_List::Ornament] = &OrnamentPalette::SingleInstance();
				paletteList[(int)Palette_List::Region] = &RegionPalette::SingleInstance();
				paletteList[(int)Palette_List::Wave] = &WavePalette::SingleInstance();

				SetCurrentPalette(paletteList[(int)Palette_List::Terrain]);
			}

			void PaletteManager::Clear()
			{
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
