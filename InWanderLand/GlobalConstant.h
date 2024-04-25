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

		GC<float>	playerOneQSkillDelay;
		GC<int>		playerOneQSkillTimingFrame;
		GC<float>	playerOneWSkillDelay;
		GC<int>		playerOneWSkillTimingFrame;

		GC<float>	playerTwoQSkillDelay;
		GC<int>		playerTwoQSkillTimingFrame;
		GC<float>	playerTwoWSkillDelay;
		GC<int>		playerTwoWSkillTimingFrame;

		GC<float>	playerThreeQSkillDelay;
		GC<int>		playerThreeQSkillTimingFrame;
		GC<float>	playerThreeWSkillDelay;
		GC<int>		playerThreeWSkillTimingFrame;

		GC<float>	eliteMeleeEnemySkillDelay;
		GC<int>		eliteMeleeEnemySkillTimingFrame;
		GC<float>	eliteRangedEnemySkillDelay;
		GC<int>		eliteRangedEnemySkillTimingFrame;

		GC<float>	bossSkillOneDelay;
		GC<int>		bossSkillOneTimingFrame;
		GC<float>	bossSkillTwoDelay;
		GC<int>		bossSkillTwoTimingFrame;
		GC<float>	bossSkillThreeDelay;
		GC<int>		bossSkillThreeTimingFrame;
		GC<float>	bossSkillFourDelay;
		GC<int>		bossSkillFourTimingFrame;

		GC<float>	spikeTrapSkillDelay;
		GC<int>		spikeTrapSkillTimingFrame;
		GC<float>	chessTrapSkillDelay;
		GC<int>		chessTrapSkillTimingFrame;
		GC<float>	triggeredTrapSkillDelay;
		GC<int>		triggeredTrapSkillTimingFrame;

		GC<float>	robinQSkillCoolTime;
		GC<float>	robinESkillCoolTime;

		GC<float>	ursulaQSkillCoolTime;
		GC<float>	ursulaESkillCoolTime;

		GC<float>	hanselQSkillCoolTime;
		GC<float>	hanselESkillCoolTime;

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



