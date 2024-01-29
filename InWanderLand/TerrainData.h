/// 2023. 11. 23 김상준
/// IEditableData 의 구체화된 클래스
/// 지형

#pragma once

#include "IEditableData.h"
#include "Terrain_TemplateData.h"

#include <memory>
#include <string>

namespace application
{
    namespace editor
    {
        class TemplateDataManager;
    }
}

namespace application
{
    namespace editor
    {
        class TerrainData;

        struct POD_Terrain
        {
            Terrain_TemplateData* templateData = nullptr;

            TO_JSON(POD_Terrain)
                FROM_JSON(POD_Terrain)
        };

        class TerrainData
            : public IEditableData
        {
            friend class InstanceManager;

        public:
            virtual bool EnterDataFromTemplate() override;
            virtual ITemplateData* GetTemplateData() override;
            virtual bool SetTemplateData(const std::string& dataName) override;
            virtual IEditableData* Clone() const override;
            virtual palette::PaletteInstance* ApplyAsPaletteInstance()override { return nullptr; };

            POD_Terrain pod;

        protected:
            virtual bool PreEncoding(json& data) const override;
            virtual bool PostEncoding(json& data) const override;
            virtual bool PreDecoding(const json& data) override;
            virtual bool PostDecoding(const json& data) override;

        private:
            static TemplateDataManager& templateDataManager;

            TerrainData();
            TerrainData(const std::string& name);
            TerrainData(const TerrainData& prototype);
            TerrainData& operator=(const TerrainData& prototype);
        };
    }
}
