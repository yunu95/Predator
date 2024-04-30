#include "ResurrectionSystem.h"

void ResurrectionSystem::Start()
{
	isSingletonComponent = true;
}

void ResurrectionSystem::PlayFunction()
{
	this->SetActive(true);
	if (isOncePaused)
	{
		Start();
	}
}

void ResurrectionSystem::StopFunction()
{

}
