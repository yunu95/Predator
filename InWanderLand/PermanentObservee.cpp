#include "PermanentObservee.h"
#include "ContentsObserver.h"

PermanentObservee::~PermanentObservee()
{
	ContentsObserver::Instance().RemoveObservee(this);
}

PermanentObservee::PermanentObservee()
	: ContentsObservee()
{
	ContentsObserver::Instance().RegisterObservee(this);
}