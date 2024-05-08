#include "PermanentObservee.h"
#include "ContentsObserver.h"

PermanentObservee::PermanentObservee()
{
	isSingletonComponent = true;
	ContentsObserver::Instance().RegisterObservee(this);
}