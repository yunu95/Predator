#include "Identifiable.h"

namespace application
{
	namespace editor
	{
		std::string UUID_To_String(UUID uuid)
		{
			char* id_char;
			if (UuidToStringA(&uuid, (RPC_CSTR*)&id_char) != RPC_S_OK)
			{
				// ¿À·ù ½Ã ºó string ¹İÈ¯
				return std::string();
			}

			return std::string(id_char);
		}

		UUID String_To_UUID(std::string str)
		{
			UUID uuid;
			if (UuidFromStringA((RPC_CSTR)str.c_str(), &uuid) != RPC_S_OK)
			{
				// ¿À·ù ½Ã ºó UUID ¹İÈ¯
				return UUID();
			}

			return uuid;
		}

		Identifiable::Identifiable()
		{
			UuidCreate(&id);
		}

		Identifiable::Identifiable(const UUID& id)
		{
			this->id = id;
		}

		UUID Identifiable::GetUUID() const
		{
			return id;
		}

		void Identifiable::SetUUID(const UUID& id)
		{
			this->id = id;
		}
	}
}
