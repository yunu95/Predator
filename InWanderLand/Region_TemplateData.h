#pragma once
/// 2023. 11. 23 김상준
/// ITemplateData 의 구체화된 클래스
/// 지형

#pragma once

#include "ITemplateData.h"

namespace application
{
    namespace editor
    {
        class Region_TemplateData;

        struct POD_Region_TemplateData
        {
            std::vector<std::pair<int, int>> coordinates;
            TO_JSON(POD_Region_TemplateData)
                FROM_JSON(POD_Region_TemplateData)
        };

        class Region_TemplateData
            :public ITemplateData
        {
            friend class RegionData;
            friend class TemplateDataManager;

        public:
            virtual std::string GetDataKey() const override;

            POD_Region_TemplateData pod;

        protected:
            virtual bool PreEncoding(json& data) const override;
            virtual bool PostEncoding(json& data) const override;
            virtual bool PreDecoding(const json& data) override;
            virtual bool PostDecoding(const json& data) override;

        private:
            Region_TemplateData();
            Region_TemplateData(const Region_TemplateData& prototype);
            Region_TemplateData& operator=(const Region_TemplateData& prototype);
        };
    }
}
