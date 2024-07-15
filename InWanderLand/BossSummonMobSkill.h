#pragma once
#include "Skill.h"

namespace BossSummon
{
	class LeftFrame;
	class RightFrame;
}

struct POD_BossSummonMobSkill
{
	float skillDuration = -1.0f;

	int leftNormalMeleeEnemyCount = 1;
	int leftWeakMeleeEnemyCount = 1;
	int leftEliteMeleeEnemyCount = 1;
	
	int leftNormalRangedCount = 1;
	int leftWeakRangedCount = 1;
	int leftWeakApproachRangedCount = 1;
	int leftKitingRangedCount = 1;
	int leftSiegeRangedCount = 1;
	int leftEliteRangedEnemyCount = 1;
	/// LeftFrame 기준 소환 위치의 offset 입니다.
	/// -x, +z
	float leftSummonOffset_x = 10.0f;
	float leftSummonOffset_z = 10.0f;
	float leftNoiseRadius = 5.0f;

	int rightNormalMeleeEnemyCount = 1;
	int rightWeakMeleeEnemyCount = 1;
	int rightEliteMeleeEnemyCount = 1;
	int rightNormalRangedCount = 1;
	int rightWeakRangedCount = 1;
	int rightWeakApproachRangedCount = 1;
	int rightKitingRangedCount = 1;
	int rightSiegeRangedCount = 1;
	int rightEliteRangedEnemyCount = 1;

	/// RightFrame 기준 소환 위치의 offset 입니다.
	/// +x, +z
	float rightSummonOffset_x = 10.0f;
	float rightSummonOffset_z = 10.0f;
	float rightNoiseRadius = 5.0f;

	float summonPeriod = 15.0f;

	TO_JSON(POD_BossSummonMobSkill)
		FROM_JSON(POD_BossSummonMobSkill)
};

class Unit;
class BossSummonMobSkill : public Skill
{
public:
	BossSummonMobSkill() {}
	virtual SkillType::Enum GetSkillType() { return SkillType::Enum::BossSkill_Three; }
	virtual coroutine::Coroutine operator()() override;
	virtual void OnInterruption() override;

	static POD_BossSummonMobSkill pod;

	static void SetLeftFrame(BossSummon::LeftFrame* leftFrame);
	static void SetRightFrame(BossSummon::RightFrame* rightFrame);

	static std::weak_ptr<Unit> GetLeftFrameUnit();
	static std::weak_ptr<Unit> GetRightFrameUnit();

	static void OnBossAppear();
	static void OnBossDie();

	virtual void OnPause() override;
	virtual void OnResume() override;

private:
	coroutine::Coroutine StartSummonTimer();
	coroutine::Coroutine SpawningFieldEffect(std::weak_ptr<BossSummonMobSkill> skill);

	yunuGI::IAnimation* skillAnim = nullptr;

	static std::weak_ptr<coroutine::Coroutine> summonCoroutine;
	std::weak_ptr<coroutine::Coroutine> effectCoroutine;
	std::weak_ptr<ManagedFBX> stepEffect;
	std::weak_ptr<yunutyEngine::graphics::Animator> animator;
	std::weak_ptr<VFXAnimator> stepEffectAnimator;

	static BossSummon::LeftFrame * leftFrame;
	static BossSummon::RightFrame* rightFrame;

	float skillSpeed = 1.f;
};





template <>
bool SkillPodFieldPreEncoding<SkillType::Enum::BossSkill_Three>(json& data)
{
	application::FieldPreEncoding<boost::pfr::tuple_size_v<POD_BossSummonMobSkill>>(BossSummonMobSkill::pod, data["POD"]);
	return true;
}

template <>
bool SkillPodFieldPostEncoding<SkillType::Enum::BossSkill_Three>(json& data)
{
	application::FieldPostEncoding<boost::pfr::tuple_size_v<POD_BossSummonMobSkill>>(BossSummonMobSkill::pod, data["POD"]);
	return true;
}

template <>
bool SkillPodFieldPreDecoding<SkillType::Enum::BossSkill_Three>(const json& data)
{
	application::FieldPreDecoding<boost::pfr::tuple_size_v<POD_BossSummonMobSkill>>(BossSummonMobSkill::pod, data["POD"]);
	return true;
}

template <>
bool SkillPodFieldPostDecoding<SkillType::Enum::BossSkill_Three>(const json& data)
{
	application::FieldPostDecoding<boost::pfr::tuple_size_v<POD_BossSummonMobSkill>>(BossSummonMobSkill::pod, data["POD"]);
	return true;
}
