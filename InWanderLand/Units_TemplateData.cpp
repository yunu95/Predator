#include "Units_TemplateData.h"

#include "TemplateDataManager.h"

namespace Application
{
	namespace Editor
	{
		std::shared_ptr<TemplateData> Units_TemplateData::Clone() const
		{
			return std::shared_ptr<TemplateData>(new Units_TemplateData(*this));
		}

		bool Units_TemplateData::PreEncoding(json& data) const
		{
			return true;
		}

		bool Units_TemplateData::PostEncoding(json& data) const
		{
			return true;
		}

		bool Units_TemplateData::PreDecoding(const json& data)
		{
			return true;
		}

		bool Units_TemplateData::PostDecoding(const json& data)
		{
			return true;
		}

		Units_TemplateData::Units_TemplateData()
			: pod()
		{

		}

		Units_TemplateData::Units_TemplateData(const Units_TemplateData& prototype)
			: TemplateData(prototype), pod(prototype.pod)
		{

		}

		Units_TemplateData& Units_TemplateData::operator=(const Units_TemplateData& prototype)
		{
			TemplateData::operator=(prototype);
			pod = prototype.pod;
			return *this;
		}
	}
}

