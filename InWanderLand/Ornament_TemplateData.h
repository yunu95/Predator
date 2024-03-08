/// 2023. 11. 23 김상준
/// ITemplateData 의 구체화된 클래스
/// 장식물

#pragma once

#include "ITemplateData.h"
#include "PodStructs.h"

#include <string>
#include <vector>

namespace application
{
	namespace editor
	{
		class Ornament_TemplateData;
		
		struct POD_Ornament_TemplateData
		{
			std::string fbxName = std::string();

			TO_JSON(POD_Ornament_TemplateData)
			FROM_JSON(POD_Ornament_TemplateData)
		};

		class Ornament_TemplateData
			:public ITemplateData
		{
			friend class OrnamentData;
			friend class TemplateDataManager;

		public:
			virtual ~Ornament_TemplateData();

			virtual std::string GetDataKey() const override;
			virtual void SetDataResourceName(std::string fbxName) override;
			virtual std::string GetDataResourceName() const override;

			POD_Ornament_TemplateData pod;

		protected:
			virtual bool PreEncoding(json& data) const override;
			virtual bool PostEncoding(json& data) const override;
			virtual bool PreDecoding(const json& data) override;
			virtual bool PostDecoding(const json& data) override;

		private:
			Ornament_TemplateData();
			Ornament_TemplateData(const Ornament_TemplateData& prototype);
			Ornament_TemplateData& operator=(const Ornament_TemplateData& prototype);
		};
	}
}
