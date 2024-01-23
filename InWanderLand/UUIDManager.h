/// 2023. 12. 19 김상준
/// UUID 를 통해 포인터를 저장하고 획득할 수 있는 Manager 클래스
/// 따로 검증 절차를 거치지 않고 void* 형태로 캐스팅하여 사용함

#pragma once

#include "Singleton.h"
#include "Identifiable.h"

#include <unordered_map>
#include <type_traits>

namespace application
{
	namespace editor
	{
		class UUIDManager
			: public Singleton<UUIDManager>
		{
			friend class Singleton<UUIDManager>;

		public:
			bool RegisterUUIDWithPointer(const UUID& uuid, Identifiable* pointer);

			template <typename T> requires std::is_pointer_v<T>
			T GetPointerFromUUID(const UUID& uuid)
			{
				return static_cast<T>(uuidMap[uuid]);
			}

			bool EraseUUID(const UUID& uuid);
			void Clear();

		private:
			UUIDManager() = default;

			std::unordered_map<const UUID, Identifiable*> uuidMap;
		};
	}
}
