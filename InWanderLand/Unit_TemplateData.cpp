#include "InWanderLand.h"
#include "Unit_TemplateData.h"

#include "TemplateDataManager.h"
#include "InstanceManager.h"
#include "UnitBrush.h"
#include "UnitData.h"

namespace application
{
    namespace editor
    {
        Unit_TemplateData::~Unit_TemplateData()
        {

        }

        std::string Unit_TemplateData::GetDataKey() const
        {
            return TemplateDataManager::GetSingletonInstance().GetDataKey(this);
        }

        void Unit_TemplateData::SetDataResourceName(std::string fbxName)
        {
            if (pod.skinnedFBXName == fbxName)
                return;

            pod.skinnedFBXName = fbxName;

            palette::UnitBrush::Instance().ChangeBrushResource(this->GetDataKey(), fbxName);

            for (auto each : InstanceManager::GetSingletonInstance().GetList<UnitData>())
            {
                if (each->pod.templateData->GetDataKey() == this->GetDataKey())
                {
                    each->OnDataResourceChange(fbxName);
                }
            }

        }

        std::string Unit_TemplateData::GetDataResourceName() const
        {
            return pod.skinnedFBXName;
        }

        bool Unit_TemplateData::EnterDataFromGlobalConstant()
        {
            auto& data = GlobalConstant::GetSingletonInstance().pod;
            return true;
        }

        bool Unit_TemplateData::PreEncoding(json& data) const
        {
            FieldPreEncoding<boost::pfr::tuple_size_v<POD_Unit_TemplateData>>(pod, data["POD"]);

            return true;
        }

        bool Unit_TemplateData::PostEncoding(json& data) const
        {
            FieldPostEncoding<boost::pfr::tuple_size_v<POD_Unit_TemplateData>>(pod, data["POD"]);

            return true;
        }

        bool Unit_TemplateData::PreDecoding(const json& data)
        {
            FieldPreDecoding<boost::pfr::tuple_size_v<POD_Unit_TemplateData>>(pod, data["POD"]);
            FillAnimationBlendMapDefault();
            return true;
        }

        bool Unit_TemplateData::PostDecoding(const json& data)
        {
            FieldPostDecoding<boost::pfr::tuple_size_v<POD_Unit_TemplateData>>(pod, data["POD"]);
            EnterDataFromGlobalConstant();
            return true;
        }

        Unit_TemplateData::Unit_TemplateData()
            : ITemplateData(), pod()
        {
            FillAnimationBlendMapDefault();
            EnterDataFromGlobalConstant();
        }

        Unit_TemplateData::Unit_TemplateData(const Unit_TemplateData& prototype)
            : ITemplateData(prototype), pod(prototype.pod)
        {
            EnterDataFromGlobalConstant();
        }

        Unit_TemplateData& Unit_TemplateData::operator=(const Unit_TemplateData& prototype)
        {
            ITemplateData::operator=(prototype);
            pod = prototype.pod;
            return *this;
        }
        void Unit_TemplateData::FillAnimationBlendMapDefault()
        {
            for (int i = 1; i < (int)UnitAnimType::End; i++)
            {
                for (int j = 1; j < (int)UnitAnimType::End; j++)
                {
                    if (i == j)
                    {
                        continue;
                    }
                    if (!pod.animationBlendMap.contains(std::pair(i, j)))
                    {
                        pod.animationBlendMap[std::pair(i, j)] = defaultBlendDuration;
                    }
                }
            }
        }
    }
}
