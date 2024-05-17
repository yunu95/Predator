#include "ContentsObservee.h"
#include "ContentsObserver.h"

ContentsObservee::ContentsObservee()
	: yunutyEngine::Component()
{
	ContentsObserver::Instance().RegisterObservee(this);
}

