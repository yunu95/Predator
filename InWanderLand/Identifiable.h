/// 2023. 11. 21 김상준
/// UUID 를 사용하여 관리가 될 대상이 상속받을 클래스

#pragma once

#include "UUID.h"

#include <string>

namespace application
{
	class UUIDManager;
}

namespace application
{
	std::string UUID_To_String(UUID uuid);
	UUID String_To_UUID(std::string str);

	class Identifiable
	{
	public:
		virtual ~Identifiable();

		Identifiable();
		Identifiable(const UUID& id);

		UUID GetUUID() const;

		// SetUUID 를 통해 UUID 를 바꾸어야만 UUIDManager 에서 변경사항이 적용되므로 주의
		void SetUUID(const UUID& id);

	protected:
		UUID id;

	private:
		static UUIDManager& uuidManager;
	};
}
