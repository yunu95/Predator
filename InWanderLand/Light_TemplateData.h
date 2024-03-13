/// 2024. 03. 13 김상준
/// ITemplateData 의 구체화된 클래스
/// Light

#pragma once

#include "ITemplateData.h"
#include "GlobalConstant.h"

namespace application
{
    namespace editor
    {
        class Light_TemplateData;

        enum class LightType
        {
            None = 0,
            Directional,
            Point,
        };

        struct POD_Light_TemplateData
        {
            LightType type = LightType::None;

            TO_JSON(POD_Light_TemplateData)
            FROM_JSON(POD_Light_TemplateData)
        };

        class Light_TemplateData
            :public ITemplateData
        {
            friend class LightData;
            friend class TemplateDataManager;

        public:
            virtual std::string GetDataKey() const override;
            virtual void SetDataResourceName(std::string fbxName) override {}
            virtual std::string GetDataResourceName() const override { return std::string(); }
            virtual bool EnterDataFromGlobalConstant() override { return true; }

            POD_Light_TemplateData pod;

        protected:
            virtual bool PreEncoding(json& data) const override;
            virtual bool PostEncoding(json& data) const override;
            virtual bool PreDecoding(const json& data) override;
            virtual bool PostDecoding(const json& data) override;

        private:
            Light_TemplateData();
            Light_TemplateData(const Light_TemplateData& prototype);
            Light_TemplateData& operator=(const Light_TemplateData& prototype);
        };
    }
}
