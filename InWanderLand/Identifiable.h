/// 2023. 11. 21 김상준
/// UUID 를 사용하여 관리가 될 대상이 상속받을 클래스

#pragma once

#include "UUID.h"

#include <string>

namespace Application
{
	namespace Editor
	{
		std::string UUID_To_String(UUID uuid);
		UUID String_From_UUID(std::string str);

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
