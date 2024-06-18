#include "ITacticObject.h"

std::unordered_set<ITacticObject*> ITacticObject::tacticObjects = std::unordered_set<ITacticObject*>();

void ITacticObject::OnPauseAll()
{
	for (auto each : tacticObjects)
	{
		each->OnPause();
	}
}

void ITacticObject::OnResumeAll()
{
	for (auto each : tacticObjects)
	{
		each->OnResume();
	}
}

ITacticObject::ITacticObject()
{
	tacticObjects.insert(this);
}

ITacticObject::~ITacticObject()
{
 	tacticObjects.erase(this);
}
