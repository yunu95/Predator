/// 2024. 04. 15 김상준
/// Cinematic 관련 처리를 위한 Singleton Manager 입니다.

#pragma once

#include "YunutyEngine.h"
#include "SingletonComponent.h"

namespace application
{
	class CinematicManager
		: public Component, public SingletonComponent<CinematicManager>
	{
		
	};
}

