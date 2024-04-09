#include "Trigger_GameStart.h"

#include "Application.h"

namespace application
{
	void Trigger_GameStart::LinkCallback()
	{
		// Trigger_GameStart는 게임 시작시 바로 시작되는 트리거이므로,
		// 특별히 어떤 콜백에 PullTrigger 함수 호출을 달 필요 없이 바로 실행해버립니다.
		// 만약 이런 예외케이스를 정의하고 싶지 않은 경우
		// 게임시작 직후 실행되는 콜백리스트를 외부에 정의하고,
		// 그 콜백 리스트에 이 트리거를 호출하도록 만들면 됩니다.
		PullTrigger();
	}

	bool Trigger_GameStart::PreEncoding(json& data) const
	{
		return true;
	}

	bool Trigger_GameStart::PostEncoding(json& data) const
	{
		return true;
	}

	bool Trigger_GameStart::PreDecoding(const json& data)
	{
		return true;
	}

	bool Trigger_GameStart::PostDecoding(const json& data)
	{
		return true;
	}
}
