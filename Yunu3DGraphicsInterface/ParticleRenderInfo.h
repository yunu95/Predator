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

		// 입자 단위 크기 변경
		float startScale;
		float curScale;
		float endScale;

		// 입자 단위 회전 변경
		float particleAngle;

		float lifeTime;
		float curLifeTime;

		float alpha = 1.f;

		void Reset()
		{
			position = Vector3(0, 0, 0);
			//direction = Vector3(0, 0, 0);
			offsetTM = Matrix4x4{};
			curScale = 0.f;
			lifeTime = 0.f;
			curLifeTime = 0.f;
			particleAngle = 0.f;
			alpha = 1.f;
		}
	};

}
