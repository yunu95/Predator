/// 2023. 10. 11 김상준
/// Contents 제작을 위한 레이어

#pragma once

#include "Layer.h"

namespace Application
{
	namespace Contents
	{
		class ContentsLayer
			: public Layer
		{
		public:
			virtual void Initialize() override;
			//virtual void EventProgress() override;
			virtual void Update(float ts) override;
			virtual void GUIProgress() override;
			virtual void Finalize() override;
		};
	}
}
