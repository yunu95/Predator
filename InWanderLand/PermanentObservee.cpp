#include "PermanentObservee.h"
#include "ContentsObserver.h"

PermanentObservee::PermanentObservee()
	: ContentsObservee()
{
	ContentsObserver::Instance().RegisterObservee(this);
}