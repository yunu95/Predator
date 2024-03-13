/// 2024. 03. 12 김상준
/// ITemplateData 의 구체화된 클래스
/// 카메라

#pragma once

#include "ITemplateData.h"
#include "GlobalConstant.h"

namespace application
{
    namespace editor
    {
        class Camera_TemplateData;

        struct POD_Camera_TemplateData
        {
            TO_JSON(POD_Camera_TemplateData)
            FROM_JSON(POD_Camera_TemplateData)
        };

        class Camera_TemplateData
            :public ITemplateData
        {
            friend class CameraData;
            friend class TemplateDataManager;

        public:
            virtual std::string GetDataKey() const override;
            virtual void SetDataResourceName(std::string fbxName) {};
            virtual std::string GetDataResourceName() const override { return std::string(); };
            virtual bool EnterDataFromGlobalConstant() override { return true; };

            POD_Camera_TemplateData pod;

        protected:
            virtual bool PreEncoding(json& data) const override;
            virtual bool PostEncoding(json& data) const override;
            virtual bool PreDecoding(const json& data) override;
            virtual bool PostDecoding(const json& data) override;

        private:
            Camera_TemplateData();
            Camera_TemplateData(const Camera_TemplateData& prototype);
            Camera_TemplateData& operator=(const Camera_TemplateData& prototype);
        };
    }
}
