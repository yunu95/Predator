#include "Identifiable.h"

namespace Application
{
	namespace Editor
	{
		Identifiable::Identifiable()
		{
			UuidCreate(&id);
		}

		Identifiable::Identifiable(const UUID& id)
		{
			this->id = id;
		}

		UUID Identifiable::GetUUID()
		{
			return id;
		}

		void Identifiable::SetUUID(const UUID& id)
		{
			this->id = id;
		}
	}
}
