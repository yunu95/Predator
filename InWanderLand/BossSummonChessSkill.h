#pragma once
#include "Skill.h"

class IInteractableComponent;

struct POD_BossSummonChessSkill
{
	/// 보스방 기준, 좌측 하단의 칸 중심 위치입니다.
	application::POD_Vector3<float> pivotPos = { -1553.66f, 0, 52.06f };
	int horizontalSpaces = 18;
	int verticalSpaces = 20;

	/// 유닛이 중앙이라고 했을 때, 배치할 수 있는 주변 칸의 반경을 나타냅니다.
	/// [1 : 3x3 / 2 : 5x5 / 3 : 7x7 ...]
	int rectUnitRadius = 2;
	int summonCount = 3;
	float offset_Y = 6;
	float summonTime = 0.5;
	float chessSummonedExplosionDelay = 1.5f;

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

private:
	coroutine::Coroutine SummonChess(Vector2i index);
	Vector2i GetPlaceableIndex(Vector3d pos);
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
