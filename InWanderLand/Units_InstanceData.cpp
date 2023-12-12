#include "Units_InstanceData.h"

#include "Units_TemplateData.h"

namespace Application
{
	namespace Editor
	{
		bool Units_InstanceData::EnterDataFromTemplate(const std::shared_ptr<TemplateData>& templateData)
		{
#pragma region Check
			auto ptr = std::dynamic_pointer_cast<Units_TemplateData>(templateData);
			if (ptr == nullptr)
			{
				return false;
			}
#pragma endregion It was included for review, but it appears that it can be ultimately excluded \
			because it includes a dynamic casting.

			// 템플릿으로부터 초기화되는 데이터들 처리 영역
			auto dataPtr = std::dynamic_pointer_cast<Units_TemplateData>(templateData);
			pod.currentHP = dataPtr->pod.maxHP;

			return true;
		}

		bool Units_InstanceData::PreEncoding(json& data) const
		{
			return true;
		}

		bool Units_InstanceData::PostEncoding(json& data) const
		{
			return true;
		}

		bool Units_InstanceData::PreDecoding(const json& data)
		{
			return true;
		}

		bool Units_InstanceData::PostDecoding(const json& data)
		{
			return true;
		}
	}
}
