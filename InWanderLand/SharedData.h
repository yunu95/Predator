/// 2023. 11. 14 김상준
/// 인스턴스화 되는 데이터 외의 모든 데이터를 담고 있는 클래스

#pragma once

#include "Identifiable.h"
#include "Storable.h"

#include <memory>

namespace Application
{
	namespace Editor
	{
		class SharedData
			: public Identifiable, public Storable
		{
		public:
			virtual bool PreEncoding(json& data) const = 0;
			virtual bool PostEncoding(json& data) const = 0;
			virtual bool PreDecoding(const json& data) = 0;
			virtual bool PostDecoding(const json& data) = 0;

			virtual std::shared_ptr<SharedData> Clone() = 0;
		};
	}
}
