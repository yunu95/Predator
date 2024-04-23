/// 2024. 04. 23 김상준
/// Particle 제작에 대한 데이터를 저장할 클래스

#pragma once

#include "YunutyEngine.h"
#include <string>

namespace application
{
	class ParticleToolData
	{
	public:
		/// 저장할 데이터들
		std::string name = "None";
		


	private:

	};

	class ParticleToolInstance
	{
	public:
		/// 저장할 데이터들
		std::string name = "None";
		Vector3d offsetPos = Vector3d();
		Quaternion rotation = Quaternion();
		Vector3d scale = Vector3d();
		

	private:

	};
}
