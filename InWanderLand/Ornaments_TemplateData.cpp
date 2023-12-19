#include "Ornaments_TemplateData.h"

#include "TemplateDataManager.h"

namespace Application
{
	namespace Editor
	{
		bool Ornaments_TemplateData::PreEncoding(json& data) const
		{
			return true;
		}

		bool Ornaments_TemplateData::PostEncoding(json& data) const
		{
			return true;
		}

		bool Ornaments_TemplateData::PreDecoding(const json& data)
		{
			return true;
		}

		bool Ornaments_TemplateData::PostDecoding(const json& data)
		{
			return true;
		}

		Ornaments_TemplateData::Ornaments_TemplateData()
		{

		}

		Ornaments_TemplateData::Ornaments_TemplateData(const Ornaments_TemplateData& prototype)
			: TemplateData(prototype)
		{

		}

		Ornaments_TemplateData& Ornaments_TemplateData::operator=(const Ornaments_TemplateData& prototype)
		{
			TemplateData::operator=(prototype);
			return *this;
		}
	}
}

