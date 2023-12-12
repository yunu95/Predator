/// 2023. 11. 14 김상준
/// 인스턴스화 되는 데이터 외의 모든 데이터를 담고 있는 클래스
/// 기본적인 데이터를 가지고 있으며, 해당 클래스를 상속받아 추가적인 디테일을 확장할 수 있음

#pragma once

#include "Identifiable.h"
#include "Storable.h"

#include <memory>
#include <string>

namespace Application
{
	namespace Editor
	{
		struct POD_TemplateData
		{
			TO_JSON(POD_TemplateData);

			std::string name = "None";
		};

		class TemplateData
			: public Identifiable, public Storable
		{
		public:
			virtual std::shared_ptr<TemplateData> Clone() const = 0;

			// 기본 공용 데이터
			POD_TemplateData pod = POD_TemplateData();

		protected:
			virtual bool PreEncoding(json& data) const = 0;
			virtual bool PostEncoding(json& data) const = 0;
			virtual bool PreDecoding(const json& data) = 0;
			virtual bool PostDecoding(const json& data) = 0;
		};
	}
}
