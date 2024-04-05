#pragma once

#include "YunuVector3.h"

namespace yunuGI
{
	struct ParticleRenderInfo
	{
		Vector3 position;
		float scale;

		float lifeTime = 5.f;
		float curLifeTime;

		void Reset()
		{
			position = Vector3(0, 0, 0);
			curLifeTime = 0.f;
		}
	};

}
