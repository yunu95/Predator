#pragma once
#include "Skill.h"

struct POD_BossDebuffFieldSkill
{

	TO_JSON(POD_BossDebuffFieldSkill)
		FROM_JSON(POD_BossDebuffFieldSkill)
};

class BossDebuffFieldSkill : public Skill
{
public:
	BossDebuffFieldSkill() {}
	virtual SkillType::Enum GetSkillType() { return SkillType::Enum::BossSkill_Four; }
	virtual coroutine::Coroutine operator()()override;
	virtual void OnInterruption() override;

	static POD_BossDebuffFieldSkill pod;
};





template <>
bool SkillPodFieldPreEncoding<SkillType::Enum::BossSkill_Four>(json& data)
{
	application::FieldPreEncoding<boost::pfr::tuple_size_v<POD_BossDebuffFieldSkill>>(BossDebuffFieldSkill::pod, data["POD"]);
	return true;
}

template <>
bool SkillPodFieldPostEncoding<SkillType::Enum::BossSkill_Four>(json& data)
{
	application::FieldPostEncoding<boost::pfr::tuple_size_v<POD_BossDebuffFieldSkill>>(BossDebuffFieldSkill::pod, data["POD"]);
	return true;
}

template <>
bool SkillPodFieldPreDecoding<SkillType::Enum::BossSkill_Four>(const json& data)
{
	application::FieldPreDecoding<boost::pfr::tuple_size_v<POD_BossDebuffFieldSkill>>(BossDebuffFieldSkill::pod, data["POD"]);
	return true;
}

template <>
bool SkillPodFieldPostDecoding<SkillType::Enum::BossSkill_Four>(const json& data)
{
	application::FieldPostDecoding<boost::pfr::tuple_size_v<POD_BossDebuffFieldSkill>>(BossDebuffFieldSkill::pod, data["POD"]);
	return true;
}
