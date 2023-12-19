#include "Ornaments_InstanceData.h"

#include "Ornaments_TemplateData.h"

namespace Application
{
	namespace Editor
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
			return true;
		}

		bool Ornaments_InstanceData::PostEncoding(json& data) const
		{
			return true;
		}

		bool Ornaments_InstanceData::PreDecoding(const json& data)
		{
			return true;
		}

		bool Ornaments_InstanceData::PostDecoding(const json& data)
		{
			return true;
		}
	}
}
