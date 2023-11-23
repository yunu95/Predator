#include "Resource.h"

Resource::Resource()
{
	this->id = staticID++;
};

unsigned int Resource::staticID = 0;

const std::wstring& Resource::GetName() const
{
	return this->name;
};

void Resource::SetName(const std::wstring& name)
{
	this->name = name;
};

unsigned int Resource::GetID()
{
	return this->id;
};
