/// 2023. 11. 14 김상준
/// 인스턴스화 되는 데이터 외의 모든 데이터를 담고 있는 클래스
/// 기본적인 데이터를 가지고 있으며, 해당 클래스를 상속받아 추가적인 디테일을 확장할 수 있음

#pragma once

#ifdef EDITOR
#include "Identifiable.h"
#include "Storable.h"

#include <memory>
#include <string>

namespace application
{
	namespace editor
	{
		class ITemplateData
			: public Identifiable, public Storable
		{
			friend class TemplateDataManager;

		public:
			virtual ~ITemplateData() = default;

			virtual std::string GetDataKey() const = 0;

		protected:
			virtual bool PreEncoding(json& data) const = 0;
			virtual bool PostEncoding(json& data) const = 0;
			virtual bool PreDecoding(const json& data) = 0;
			virtual bool PostDecoding(const json& data) = 0;
		};
	}
}
#endif
