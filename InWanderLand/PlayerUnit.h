//#pragma once
//#include "SkillUnit.h"
//
///// <summary>
///// Player 유닛들만이 갖게 되는 특수한 상태를 관리하는 클래스.
///// </summary>
//class PlayerUnit : public SkillUnit
//{
//public:
//	virtual void Start() override;
//
//
//
//private:
//	// FSM Transition Related Members
//	bool isWaveMotionEnded{ false };
//	bool isResurrectEnded{ false };
//
//
//	// FSM Engage Functions
//	void OffsetMoveEngage();
//	void ResurrectEngage();
//	void WaveStartEngage();
//	void WaveMotionEngage();
//
//	// FSM Update Functions
//	void OffsetMoveUpdate();
//	void ResurrectUpdate();
//	void WaveStartUpdate();
//	void WaveMotionUpdate();
//
//private:
//	Unit* m_followingTargetUnit;
//	float m_followEngageDinstance{ 4.0f };			// 이 수치만큼 거리가 벌어지면 따라간다.
//	float m_stopFollowDinstance{ 2.0f };			// 이 수치만큼 거리가 좁혀지면 멈춘다.
//	bool isFollowing{ false };
//
//	bool isOffSetMoveRequested{ false };
//
//	float m_resurrectingDuration{ 5.0f };
//
//public:
//	void ReportLeaderUnitChanged(UnitType p_type);
//};

