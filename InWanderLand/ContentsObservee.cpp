#include "ContentsObservee.h"
#include "ContentsObserver.h"
#include "YunutyEngine.h"

ContentsObservee::ContentsObservee()
{
	ContentsObserver::Instance().RegisterObservee(this);
}
