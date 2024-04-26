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
		GC<int> gravitySpeed = 10;
		GC<int> magicPower = 40;

		GC<float>	playerOneQSkillDelay = 2.0f;
		GC<int>		playerOneQSkillTimingFrame = 21;
		GC<float>	playerOneWSkillDelay = 2.0f;
		GC<int>		playerOneWSkillTimingFrame = 21;

		GC<float>	playerTwoQSkillDelay = 2.0f;
		GC<int>		playerTwoQSkillTimingFrame = 21;
		GC<float>	playerTwoWSkillDelay = 2.0f;
		GC<int>		playerTwoWSkillTimingFrame = 21;

		GC<float>	playerThreeQSkillDelay = 2.0f;
		GC<int>		playerThreeQSkillTimingFrame = 21;
		GC<float>	playerThreeWSkillDelay = 2.0f;
		GC<int>		playerThreeWSkillTimingFrame = 21;

		GC<float>	eliteMeleeEnemySkillDelay = 1.0f;
		GC<int>		eliteMeleeEnemySkillTimingFrame = 21;
		GC<float>	eliteRangedEnemySkillDelay = 1.0f;
		GC<int>		eliteRangedEnemySkillTimingFrame = 21;

		GC<float>	bossSkillOneDelay = 3.0f;
		GC<int>		bossSkillOneTimingFrame = 15;
		GC<float>	bossSkillTwoDelay = 3.0f;
		GC<int>		bossSkillTwoTimingFrame = 15;
		GC<float>	bossSkillThreeDelay = 3.0f;
		GC<int>		bossSkillThreeTimingFrame = 15;
		GC<float>	bossSkillFourDelay = 3.0f;
		GC<int>		bossSkillFourTimingFrame = 15;

		GC<float>	spikeTrapSkillDelay = 3.0f;
		GC<int>		spikeTrapSkillTimingFrame = 15;
		GC<float>	chessTrapSkillDelay = 2.0f;
		GC<int>		chessTrapSkillTimingFrame = 10;
		GC<float>	triggeredTrapSkillDelay;
		GC<int>		triggeredTrapSkillTimingFrame;

		GC<float>	robinQSkillCoolTime = 2.0f;
		GC<float>	robinESkillCoolTime = 2.0f;

		GC<float>	ursulaQSkillCoolTime = 2.0f;
		GC<float>	ursulaESkillCoolTime = 2.0f;
		
		GC<float>	hanselQSkillCoolTime = 2.0f;
		GC<float>	hanselESkillCoolTime = 2.0f;

		GC<int>	maxResurrectCount;

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



