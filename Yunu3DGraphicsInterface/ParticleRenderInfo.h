#pragma once

#include "YunuVector3.h"

namespace yunuGI
{
	struct ParticleRenderInfo
	{
		Vector3 position;

		Matrix4x4 offsetTM;

		//Vector3 deltaPosition;
		//Vector3 direction;

		float startScale;
		float curScale;
		float endScale;

		float lifeTime;
		float curLifeTime;

		void Reset()
		{
			position = Vector3(0, 0, 0);
			//direction = Vector3(0, 0, 0);
			offsetTM = Matrix4x4{};
			curScale = 0.f;
			lifeTime = 0.f;
			curLifeTime = 0.f;
		}
	};

}
