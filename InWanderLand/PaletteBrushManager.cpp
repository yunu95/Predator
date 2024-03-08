#include "PaletteBrushManager.h"

#include "BrushList.h"
#include "PaletteManager.h"
#include "TemplateDataManager.h"

namespace application
{
    namespace editor
    {
        namespace palette
        {
            void PaletteBrushManager::Initialize()
            {
                brushList[(int)Palette_List::Terrain] = &TerrainBrush::Instance();
                brushList[(int)Palette_List::Unit] = &UnitBrush::Instance();
                brushList[(int)Palette_List::Ornament] = &OrnamentBrush::Instance();
                //brushList[(int)Palette_List::Region] = &RegionBrush::Instance();
                //brushList[(int)Palette_List::Wave] = &WaveBrush::Instance();
            }

            void PaletteBrushManager::Clear()
            {
                for (auto& each : brushList)
                {
                    if (each)
                    {
                        each->Clear();
                    }
                }
            }

            void PaletteBrushManager::MakeBrush()
            {
                for (auto& each : brushList)
                {
                    if (each)
                    {
                        each->CreateBrush();
                    }
                }
            }

            PaletteBrushManager::PaletteBrushManager()
                : brushList()
            {
                brushList.resize((int)Palette_List::Size);
            }
        }
    }
}
