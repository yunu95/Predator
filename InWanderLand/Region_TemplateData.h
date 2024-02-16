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
            virtual void SetDataResourceName(std::string fbxName) {};
            virtual std::string GetDataResourceName() const override { return std::string(); };
            static Region_TemplateData& GetInstance()
            {
                if (soleRegionTemplateData == nullptr)
                {
                    soleRegionTemplateData = TemplateDataManager::GetSingletonInstance().GetTemplateData<Region_TemplateData>("DefaultRegionTemplate");
                    if (soleRegionTemplateData == nullptr)
                        soleRegionTemplateData = TemplateDataManager::GetSingletonInstance().CreateTemplateData<Region_TemplateData>("DefaultRegionTemplate");
                }
                POD_Region_TemplateData pod;
                return *soleRegionTemplateData;
            }
            POD_Region_TemplateData pod;

        protected:
            virtual bool PreEncoding(json& data) const override;
            virtual bool PostEncoding(json& data) const override;
            virtual bool PreDecoding(const json& data) override;
            virtual bool PostDecoding(const json& data) override;

        private:
            static Region_TemplateData* soleRegionTemplateData;
            Region_TemplateData();
            Region_TemplateData(const Region_TemplateData& prototype);
            virtual ~Region_TemplateData() { if (this == soleRegionTemplateData) soleRegionTemplateData = nullptr; }
            Region_TemplateData& operator=(const Region_TemplateData& prototype);
        };
    }
}
