#ifdef EDITOR
#include "InWanderLand.h"
#include "Storable.h"

namespace application
{
	json Storable::Encoding() const
	{
		json data;

		PreEncoding(data);
		PostEncoding(data);

		return data;
	}

	bool Storable::Decoding(const json& data)
	{
		if (!PreDecoding(data))
		{
			return false;
		}

		if (!PostDecoding(data))
		{
			return false;
		}

		return true;
	}
}
#endif
