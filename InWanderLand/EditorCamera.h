/// 2024. 01. 04 김상준
/// Editor 에서 Scene 내용을 출력 할 카메라
/// 하나의 카메라만 사용할 것이기 때문에 Singleton 으로 구현

#pragma once

#include "Singleton.h"

namespace application
{
	namespace editor
	{
		class EditorCamera
			: public Singleton<EditorCamera>
		{
		public:

		private:

		};
	}
}

