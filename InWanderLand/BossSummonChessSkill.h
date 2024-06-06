#pragma once
#include "Skill.h"

struct POD_BossSummonChessSkill
{

	TO_JSON(POD_BossSummonChessSkill)
		FROM_JSON(POD_BossSummonChessSkill)
};

class BossSummonChessSkill : public Skill
{
public:
	BossSummonChessSkill() {}
	virtual SkillType::Enum GetSkillType() { return SkillType::Enum::BossSkill_Four; }
	virtual coroutine::Coroutine operator()()override;
	virtual void OnInterruption() override;

	static POD_BossSummonChessSkill pod;
};





template <>
bool SkillPodFieldPreEncoding<SkillType::Enum::BossSkill_Four>(json& data)
{
	application::FieldPreEncoding<boost::pfr::tuple_size_v<POD_BossSummonChessSkill>>(BossSummonChessSkill::pod, data["POD"]);
	return true;
}

template <>
bool SkillPodFieldPostEncoding<SkillType::Enum::BossSkill_Four>(json& data)
{
	application::FieldPostEncoding<boost::pfr::tuple_size_v<POD_BossSummonChessSkill>>(BossSummonChessSkill::pod, data["POD"]);
	return true;
}

template <>
bool SkillPodFieldPreDecoding<SkillType::Enum::BossSkill_Four>(const json& data)
{
	application::FieldPreDecoding<boost::pfr::tuple_size_v<POD_BossSummonChessSkill>>(BossSummonChessSkill::pod, data["POD"]);
	return true;
}

template <>
bool SkillPodFieldPostDecoding<SkillType::Enum::BossSkill_Four>(const json& data)
{
	application::FieldPostDecoding<boost::pfr::tuple_size_v<POD_BossSummonChessSkill>>(BossSummonChessSkill::pod, data["POD"]);
	return true;
}
