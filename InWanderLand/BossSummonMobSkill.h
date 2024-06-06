#pragma once
#include "Skill.h"

struct POD_BossSummonMobSkill
{

	TO_JSON(POD_BossSummonMobSkill)
		FROM_JSON(POD_BossSummonMobSkill)
};

class BossSummonMobSkill : public Skill
{
public:
	BossSummonMobSkill() {}
	virtual SkillType::Enum GetSkillType() { return SkillType::Enum::BossSkill_Three; }
	virtual coroutine::Coroutine operator()() override;
	virtual void OnInterruption() override;

	static POD_BossSummonMobSkill pod;
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
