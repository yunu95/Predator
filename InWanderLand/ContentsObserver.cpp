#include "ContentsObserver.h"
#include "ContentsObservee.h"
#include "PermanentObservee.h"

void ContentsObserver::RegisterObservee(PermanentObservee* permanentObservee)
{
	permanentObservees.push_back(permanentObservee);
	contentsObservees.resize(contentsObservees.size() - 1);
}

void ContentsObserver::RegisterObservee(ContentsObservee* p_observee)
{
	contentsObservees.push_back(p_observee);
}

void ContentsObserver::OnPlayContents()
{
	for (auto each : permanentObservees)
	{
		each->OnContentsPlay();
	}
}

void ContentsObserver::OnStopContents()
{
	for (auto each : permanentObservees)
	{
		each->OnContentsStop();
	}

	auto scene = yunutyEngine::Scene::getCurrentScene();

	for (auto each : contentsObservees)
	{
		scene->DestroyGameObject(each->GetComponent()->GetGameObject());
	}

	contentsObservees.clear();
}
