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
        class Wave_TemplateData;

        struct POD_Wave_TemplateData
        {
            TO_JSON(POD_Wave_TemplateData)
                FROM_JSON(POD_Wave_TemplateData)
        };

        class Wave_TemplateData
            :public ITemplateData
        {
            friend class WaveData;
            friend class TemplateDataManager;

        public:
            virtual std::string GetDataKey() const override;
            virtual void SetDataResourceName(std::string fbxName) {};
            virtual std::string GetDataResourceName() const override { return std::string(); };

            static Wave_TemplateData& GetInstance()
            {
                if (soleWaveTemplateData == nullptr)
                {
                    soleWaveTemplateData = TemplateDataManager::GetSingletonInstance().GetTemplateData<Wave_TemplateData>("DefaultWaveTemplate");
                    if (soleWaveTemplateData == nullptr)
                        soleWaveTemplateData = TemplateDataManager::GetSingletonInstance().CreateTemplateData<Wave_TemplateData>("DefaultWaveTemplate");
                }
                return *soleWaveTemplateData;
            }
            POD_Wave_TemplateData pod;

        protected:
            virtual bool PreEncoding(json& data) const override;
            virtual bool PostEncoding(json& data) const override;
            virtual bool PreDecoding(const json& data) override;
            virtual bool PostDecoding(const json& data) override;

        private:
            static Wave_TemplateData* soleWaveTemplateData;
            Wave_TemplateData();
            Wave_TemplateData(const Wave_TemplateData& prototype);
            Wave_TemplateData& operator=(const Wave_TemplateData& prototype);
        };
    }
}
