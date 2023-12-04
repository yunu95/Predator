/// 2023. 11. 21 김상준
/// UUID 를 사용하여 관리가 될 대상이 상속받을 클래스

#pragma once

#include "UUID.h"

namespace Application
{
	namespace Editor
	{
		class Identifiable
		{
		public:
			Identifiable();
			Identifiable(const UUID& id);

			UUID GetUUID();
			void SetUUID(const UUID& id);

		protected:
			UUID id;
		};
	}
}
