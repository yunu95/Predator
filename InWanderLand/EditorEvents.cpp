#include "EditorEvents.h"

namespace application
{
	namespace editor
	{
		unsigned long long operator | (const EventCategory& c1, const EventCategory& c2)
		{
			return static_cast<unsigned long long>(c1) | static_cast<unsigned long long>(c2);
		}

		unsigned long long operator | (const unsigned long long& c1, const EventCategory& c2)
		{
			return c1 | static_cast<unsigned long long>(c2);
		}

		unsigned long long operator | (const EventCategory& c1, const unsigned long long&& c2)
		{
			return static_cast<unsigned long long>(c1) | c2;
		}

		unsigned long long operator & (const EventCategory& c1, const EventCategory& c2)
		{
			return static_cast<unsigned long long>(c1) & static_cast<unsigned long long>(c2);
		}

		unsigned long long operator & (const unsigned long long& c1, const EventCategory& c2)
		{
			return c1 & static_cast<unsigned long long>(c2);
		}

		unsigned long long operator & (const EventCategory& c1, const unsigned long long& c2)
		{
			return static_cast<unsigned long long>(c1) & c2;
		}
	}
}
