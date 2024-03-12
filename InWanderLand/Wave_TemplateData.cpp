#include "InWanderLand.h"
#include "Wave_TemplateData.h"

namespace application
{
    namespace editor
    {
        Wave_TemplateData* Wave_TemplateData::soleWaveTemplateData{nullptr};
        std::string Wave_TemplateData::GetDataKey() const
        {
            return TemplateDataManager::GetSingletonInstance().GetDataKey(this);
        }

        bool Wave_TemplateData::EnterDataFromGlobalConstant()
        {
            auto& data = GlobalConstant::GetSingletonInstance().pod;
            return true;
        }

        bool Wave_TemplateData::PreEncoding(json& data) const
        {
            FieldPreEncoding<boost::pfr::tuple_size_v<POD_Wave_TemplateData>>(pod, data["POD"]);

            return true;
        }

        bool Wave_TemplateData::PostEncoding(json& data) const
        {
            FieldPostEncoding<boost::pfr::tuple_size_v<POD_Wave_TemplateData>>(pod, data["POD"]);

            return true;
        }

        bool Wave_TemplateData::PreDecoding(const json& data)
        {
            FieldPreDecoding<boost::pfr::tuple_size_v<POD_Wave_TemplateData>>(pod, data["POD"]);

            return true;
        }

        bool Wave_TemplateData::PostDecoding(const json& data)
        {
            FieldPostDecoding<boost::pfr::tuple_size_v<POD_Wave_TemplateData>>(pod, data["POD"]);
            EnterDataFromGlobalConstant();
            return true;
        }

        Wave_TemplateData::Wave_TemplateData()
            : ITemplateData(), pod()
        {

        }

        Wave_TemplateData::Wave_TemplateData(const Wave_TemplateData& prototype)
            :ITemplateData(prototype), pod(prototype.pod)
        {
            EnterDataFromGlobalConstant();
        }

        Wave_TemplateData& Wave_TemplateData::operator=(const Wave_TemplateData& prototype)
        {
            ITemplateData::operator=(prototype);
            pod = prototype.pod;
            return *this;
        }
    }
}