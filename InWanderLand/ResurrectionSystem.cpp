#include "ResurrectionSystem.h"

void ResurrectionSystem::Start()
{
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
