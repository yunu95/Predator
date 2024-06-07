#pragma once
#include "Skill.h"

class Interactable_ChessPawn;
class Interactable_ChessRook;
class Interactable_ChessBishop;

namespace BossSkill
{
	struct CustomCompPawn
	{
		bool operator()(const std::weak_ptr<Interactable_ChessPawn>& lp, const std::weak_ptr<Interactable_ChessPawn>& rp) const
		{
			return &lp > &rp;
		}
	};

	struct CustomCompRook
	{
		bool operator()(const std::weak_ptr<Interactable_ChessRook>& lp, const std::weak_ptr<Interactable_ChessRook>& rp) const
		{
			return &lp > &rp;
		}
	};

	struct CustomCompBishop
	{
		bool operator()(const std::weak_ptr<Interactable_ChessBishop>& lp, const std::weak_ptr<Interactable_ChessBishop>& rp) const
		{
			return &lp > &rp;
		}
	};
}

struct POD_BossSummonChessSkill
{
	/// 보스방 기준, 좌측 하단의 칸 중심 위치입니다.
	float pivotPos_x = -1553.66f;
	float pivotPos_z = 52.06f;
	int horizontalSpaces = 18;
	int verticalSpaces = 20;

	/// 유닛이 중앙이라고 했을 때, 배치할 수 있는 주변 칸의 반경을 나타냅니다.
	/// [1 : 3x3 / 2 : 5x5 / 3 : 7x7 ...]
	int rectUnitRadius = 2;
	int summonCount = 3;
	float offset_Y = 6;
	float summonTime = 2;
	bool intervalSummon = true;
	float chessSummonedExplosionDelay = 1.5f;

	TO_JSON(POD_BossSummonChessSkill)
		FROM_JSON(POD_BossSummonChessSkill)
};

class BossSummonChessSkill : public Skill
{
public:
	virtual ~BossSummonChessSkill() override;
	BossSummonChessSkill() {}
	virtual SkillType::Enum GetSkillType() { return SkillType::Enum::BossSkill_Four; }
	virtual coroutine::Coroutine operator()()override;
	virtual void OnInterruption() override;

	static POD_BossSummonChessSkill pod;

private:
	std::set<std::weak_ptr<Interactable_ChessPawn>, BossSkill::CustomCompPawn> borrowedPawns = std::set<std::weak_ptr<Interactable_ChessPawn>, BossSkill::CustomCompPawn>();
	std::set<std::weak_ptr<Interactable_ChessRook>, BossSkill::CustomCompRook> borrowedRooks = std::set<std::weak_ptr<Interactable_ChessRook>, BossSkill::CustomCompRook>();
	std::set<std::weak_ptr<Interactable_ChessBishop>, BossSkill::CustomCompBishop> borrowedBishops = std::set<std::weak_ptr<Interactable_ChessBishop>, BossSkill::CustomCompBishop>();
	coroutine::Coroutine SummonChess(std::weak_ptr<BossSummonChessSkill> skill, Vector2i index);
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
