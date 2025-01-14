/// 2023. 11. 23 김상준
/// ITemplateData 의 구체화된 클래스
/// 지형

#pragma once

#include "ITemplateData.h"
#include "GlobalConstant.h"

namespace application
{
    namespace editor
    {
        class Terrain_TemplateData;

        struct POD_Terrain_TemplateData
        {
            std::vector<std::pair<int, int>> coordinates;
            
            TO_JSON(POD_Terrain_TemplateData)
            FROM_JSON(POD_Terrain_TemplateData)
        };

        class Terrain_TemplateData
            :public ITemplateData
        {
            friend class TerrainData;
            friend class TemplateDataManager;

        public:
            virtual std::string GetDataKey() const override;
            virtual void SetDataResourceName(std::string fbxName) {};
            virtual std::string GetDataResourceName() const override { return std::string(); };
            virtual bool EnterDataFromGlobalConstant() override { return true; };

            POD_Terrain_TemplateData pod;

        protected:
            virtual bool PreEncoding(json& data) const override;
            virtual bool PostEncoding(json& data) const override;
            virtual bool PreDecoding(const json& data) override;
            virtual bool PostDecoding(const json& data) override;

        private:
            Terrain_TemplateData();
            Terrain_TemplateData(const Terrain_TemplateData& prototype);
            Terrain_TemplateData& operator=(const Terrain_TemplateData& prototype);
        };
    }
}
