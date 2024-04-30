#include "ObserveeHelper.h"

void ObserveeHelper::PlayFunction()
{

}

void ObserveeHelper::StopFunction()
{
	if (GetGameObject()->GetSelfActive())
		GetGameObject()->SetSelfActive(false);
}
