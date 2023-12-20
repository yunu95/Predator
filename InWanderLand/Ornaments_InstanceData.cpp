#include "Ornaments_InstanceData.h"

#include "Ornaments_TemplateData.h"

namespace application
{
	namespace editor
	{
		bool Ornaments_InstanceData::EnterDataFromTemplate(const TemplateData* templateData)
		{
#pragma region Check
			auto ptr = dynamic_cast<Ornaments_TemplateData*>(const_cast<TemplateData*>(templateData));
			if (ptr == nullptr)
			{
				return false;
			}
#pragma endregion It was included for review, but it appears that it can be ultimately excluded \
			because it includes a dynamic casting.

			// 템플릿으로부터 초기화되는 데이터들 처리 영역

			return true;
		}

		bool Ornaments_InstanceData::PreEncoding(json& data) const
		{
			FieldEncoding<boost::pfr::tuple_size_v<POD_InstanceData>>(InstanceData::pod, data["POD_Base"]);
			FieldEncoding<boost::pfr::tuple_size_v<POD_Ornaments_InstanceData>>(pod, data["POD"]);
			return true;
		}

		bool Ornaments_InstanceData::PostEncoding(json& data) const
		{
			return true;
		}

		bool Ornaments_InstanceData::PreDecoding(const json& data)
		{
			FieldDecoding<boost::pfr::tuple_size_v<POD_InstanceData>>(InstanceData::pod, data["POD_Base"]);
			FieldDecoding<boost::pfr::tuple_size_v<POD_Ornaments_InstanceData>>(pod, data["POD"]);
			return true;
		}

		bool Ornaments_InstanceData::PostDecoding(const json& data)
		{
			return true;
		}
	}
}
