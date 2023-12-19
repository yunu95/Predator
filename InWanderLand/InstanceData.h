/// 2023. 11. 23 김상준
/// Instance 화 되는 데이터의 실제 데이터들을 보유한 클래스
/// 기본적인 데이터를 가지고 있으며, 해당 클래스를 상속받아 추가적인 디테일을 확장할 수 있음

#pragma once

#include "Storable.h"

namespace Application
{
	namespace Editor
	{
		class TemplateData;
	}
}

namespace Application
{
	namespace Editor
	{
		struct POD_InstanceData
		{
			TO_JSON(POD_InstanceData);
			FROM_JSON(POD_InstanceData);

			std::string test = "test";
		};

		class InstanceData
			: public Storable
		{
		public:
			virtual ~InstanceData() = default;

			// 템플릿으로부터 초기화되는 데이터들을 일괄적으로 처리하는 함수
			virtual bool EnterDataFromTemplate(const TemplateData* templateData) = 0;	

			// 기본 공용 데이터
			POD_InstanceData pod = POD_InstanceData();
			int testInt = 10;

		protected:
			virtual bool PreEncoding(json& data) const = 0;
			virtual bool PostEncoding(json& data) const = 0;
			virtual bool PreDecoding(const json& data) = 0;
			virtual bool PostDecoding(const json& data) = 0;
		};
	}
}
