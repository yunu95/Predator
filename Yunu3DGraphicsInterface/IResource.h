#pragma once
#include "IUnknown.h"

#include <string>

namespace yunuGI
{
	class IResource
	{
	public:
		virtual const std::wstring& GetName() const = 0;
		virtual void SetName(const std::wstring& name) = 0;
		virtual unsigned int GetID() = 0;

	protected:
		std::wstring name;
	};
}