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
                
                // 우선 보류
                //brushList[(int)Palette_List::Cam] = &CameraBrush::Instance();
                brushList[3] = &CameraBrush::Instance();
                brushList.resize(4);

                for (auto& each : brushList)
                {
                    each->Initialize();
                }
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
