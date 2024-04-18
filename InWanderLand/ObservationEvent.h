/// 2024. 04. 17 김상준
/// ObservationTarget 과 Observer 간의 소통할 Event 목록입니다.

#pragma once

namespace application
{
	enum class ObservationEvent
	{
		None,
		Destroy,
	};
}
