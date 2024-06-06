#pragma once
#include "Skill.h"

struct POD_BossImpaleSkill
{

	TO_JSON(POD_BossImpaleSkill)
		FROM_JSON(POD_BossImpaleSkill)
};

class BossImpaleSkill : public Skill
{
public:
	BossImpaleSkill() {}
	virtual SkillType::Enum GetSkillType() { return SkillType::Enum::BossSkill_Two; }
	virtual coroutine::Coroutine operator()() override;

	static POD_BossImpaleSkill pod;
	virtual void OnInterruption() override;
};





template <>
bool SkillPodFieldPreEncoding<SkillType::Enum::BossSkill_Two>(json& data)
{
	application::FieldPreEncoding<boost::pfr::tuple_size_v<POD_BossImpaleSkill>>(BossImpaleSkill::pod, data["POD"]);
	return true;
}

template <>
bool SkillPodFieldPostEncoding<SkillType::Enum::BossSkill_Two>(json& data)
{
	application::FieldPostEncoding<boost::pfr::tuple_size_v<POD_BossImpaleSkill>>(BossImpaleSkill::pod, data["POD"]);
	return true;
}

template <>
bool SkillPodFieldPreDecoding<SkillType::Enum::BossSkill_Two>(const json& data)
{
	application::FieldPreDecoding<boost::pfr::tuple_size_v<POD_BossImpaleSkill>>(BossImpaleSkill::pod, data["POD"]);
	return true;
}

template <>
bool SkillPodFieldPostDecoding<SkillType::Enum::BossSkill_Two>(const json& data)
{
	application::FieldPostDecoding<boost::pfr::tuple_size_v<POD_BossImpaleSkill>>(BossImpaleSkill::pod, data["POD"]);
	return true;
}

