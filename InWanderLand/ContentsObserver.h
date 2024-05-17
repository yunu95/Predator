#pragma once
#include "YunutyEngine.h"

class ContentsObservee;
class PermanentObservee;

#include <vector>

class ContentsObserver : virtual public Component, public SingletonComponent<ContentsObserver>
{
public:
	void RegisterObservee(PermanentObservee* permanentObservee);
	void RegisterObservee(ContentsObservee* contentsObservee);

	/// PermanentObservee 의 경우에만 시작할 때 처리하는 OnContentsPlay 함수를 호출합니다.
	void OnPlayContents();

	/// ContentsObserver 에 등록된 Observee 들 중 PermanentObservee 를 제외한 Observee 들은
	/// Destory 되고, PermanentObservee 는 OnContentsStop 이 호출됩니다.
	void OnStopContents();

private:
	std::vector<PermanentObservee*> permanentObservees = std::vector<PermanentObservee*>();
	std::vector<ContentsObservee*> contentsObservees = std::vector<ContentsObservee*>();
};

