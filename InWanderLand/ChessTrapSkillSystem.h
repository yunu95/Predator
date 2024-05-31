//#pragma once
//#include "EnemySkillSystem.h"
//
//class ChessTrapSkillSystem : public EnemySkillSystem
//{
//public:
//	enum class ChessPieceType
//	{
//		Pawn,
//		Rook,
//		Bishop
//	};
//
//private:
//	float m_unitLength;
//	float m_elapsed{ 0.0f };
//	float m_colliderRemainDuration{ 3.0f };
//
//	SkillRequirements m_chessSkillRequirementOne;
//	SkillRequirements m_chessSkillRequirementTwo;
//
//	ChessPieceType m_chessPieceType;
//
//public:
//	virtual void Start() override;
//	virtual void Update() override;
//
//	virtual void ActivateSkill(Unit::SkillEnum p_currentSkill, Vector3d p_skillPosition) override;
//
//	void SetUnitLength(const float p_length);
//
//	void SetChessType(ChessPieceType p_type);
//
//	void SetChessSkillRequirmentOne(GameObject* p_colliderObj, GameObject* p_debugObj);
//	void SetChessSkillRequirmentTwo(GameObject* p_colliderObj, GameObject* p_debugObj);
//
//	virtual void ActivateSkillOne() override;
//};

