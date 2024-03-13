/// 2024. 03. 11 김상준
/// Content Layer 에서 사용할 공용 변수들에 대해 관리하게 될
/// Global Constant 용 POD 구조체

#pragma once

#include "Singleton.h"
#include "GCTemplate.h"
#include "Storable.h"
#include "PodStructs.h"

namespace application
{
	struct POD_GlobalConstant
	{
		GC<int> testInt = 10;
		GC<int> magic = 40;
		GC<float> wow = 50;

		TO_JSON(POD_GlobalConstant)
		FROM_JSON(POD_GlobalConstant)
	};

	class GlobalConstant
		: public Singleton<GlobalConstant>
	{
		friend class Singleton<GlobalConstant>;
	public:
		POD_GlobalConstant pod;

		bool PreEncoding(json& data) const;
		bool PreDecoding(const json& data);

	private:
		GlobalConstant() = default;
	};
}



