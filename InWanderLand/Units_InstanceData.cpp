#include "Units_InstanceData.h"

#include "Units_TemplateData.h"

namespace Application
{
	namespace Editor
	{
		bool Units_InstanceData::EnterDataFromTemplate(const TemplateData* templateData)
		{
#pragma region Check
			auto ptr = dynamic_cast<Units_TemplateData*>(const_cast<TemplateData*>(templateData));
			if (ptr == nullptr)
			{
				return false;
			}
#pragma endregion It was included for review, but it appears that it can be ultimately excluded \
			because it includes a dynamic casting.

			// 템플릿으로부터 초기화되는 데이터들 처리 영역
			auto dataPtr = static_cast<Units_TemplateData*>(const_cast<TemplateData*>(templateData));
			pod.currentHP = dataPtr->pod.maxHP;

			return true;
		}

		bool Units_InstanceData::PreEncoding(json& data) const
		{
			FieldEncoding<boost::pfr::tuple_size_v<POD_InstanceData>>(InstanceData::pod, data["POD_Base"]);
			FieldEncoding<boost::pfr::tuple_size_v<POD_Units_InstanceData>>(pod, data["POD"]);
			return true;
		}

		bool Units_InstanceData::PostEncoding(json& data) const
		{
			data["testInt"] = testInt;
			data["testDouble"] = testDouble;
			return true;
		}

		bool Units_InstanceData::PreDecoding(const json& data)
		{
			FieldDecoding<boost::pfr::tuple_size_v<POD_InstanceData>>(InstanceData::pod, data["POD_Base"]);
			FieldDecoding<boost::pfr::tuple_size_v<POD_Units_InstanceData>>(pod, data["POD"]);
			return true;
		}

		bool Units_InstanceData::PostDecoding(const json& data)
		{
			testInt = data["testInt"];
			testDouble = data["testDouble"];
			return true;
		}
	}
}
