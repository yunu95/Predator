/// 2024. 05. 09 김상준
/// ITemplateData 의 구체화된 클래스
/// 상호작용 가능 Object

#pragma once

#include "ITemplateData.h"
#include "PodStructs.h"
#include "GlobalConstant.h"

#include <string>
#include <vector>

namespace application
{
	namespace editor
	{
		class Interactable_TemplateData;

		struct POD_Interactable_TemplateData
		{
			std::string fBXName = std::string();
			bool activeInteractable = false;
			bool repetition = false;
			float delayTime = 1.5;
			float particleEffectTime = 2;
			float damage = 0;

			/// GlobalConstant

			TO_JSON(POD_Interactable_TemplateData)
			FROM_JSON(POD_Interactable_TemplateData)
		};

		class Interactable_TemplateData
			:public ITemplateData
		{
			friend class InteractableData;
			friend class TemplateDataManager;

		public:
			virtual ~Interactable_TemplateData();

			virtual std::string GetDataKey() const override;
			virtual void SetDataResourceName(std::string fbxName) override;
			virtual std::string GetDataResourceName() const override;
			virtual bool EnterDataFromGlobalConstant() override;

			POD_Interactable_TemplateData pod;

			static std::vector<std::string>& GetInteractableFBXNameList() { return interactableFBXNameList; }

		protected:
			virtual bool PreEncoding(json& data) const override;
			virtual bool PostEncoding(json& data) const override;
			virtual bool PreDecoding(const json& data) override;
			virtual bool PostDecoding(const json& data) override;

		private:
			static std::vector<std::string> interactableFBXNameList;

			Interactable_TemplateData();
			Interactable_TemplateData(const Interactable_TemplateData& prototype);
			Interactable_TemplateData& operator=(const Interactable_TemplateData& prototype);
		};
	}
}
