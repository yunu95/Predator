#pragma once
#include "YunutyEngine.h"
#include "FSM.h"
#include "Timer.h"
#include "Dotween.h"
#include <list>
#include "RobinSkillDevelopmentSystem.h"
#include "DummyComponent.h"

class UnitProductor;
enum class SkillPreviewMesh;

/// <summary>
/// 유닛들이 공유하는 멤버.
/// </summary>
class Unit : public Component
{
public:
	// 사용 시 주의점 : 마지막에는 Death와 StateEnd가 순서대로 들어가 있을 것!
	enum class UnitState
	{
		Idle,
		Move,
		Chase,
		Attack,
		AttackMove,
		Skill,
		Paralysis,
		Death,
		OffsetMove,
		WaveEngage,
		StateEnd
	};

	enum class AttackType
	{
		Melee,
		Ranged
	};

	enum class UnitType
	{
		Warrior = 1,
		Magician,
		Healer,
		AllPlayers,
		MeleeEnemy,
		RangedEnemy,
		Boss,
		EnemySpawnGate
	};

	enum class UnitSide
	{
		Player,
		Enemy
	};

	enum class SkillEnum
	{
		Q,
		W,
		BossSkillOne,
		BossSkillTwo,
		BossSkillThree,
		BossSkillFour
	};

	struct BaseUnitAnimationStruct
	{
		yunuGI::IAnimation* m_idleAnimation;
		yunuGI::IAnimation* m_walkAnimation;
		yunuGI::IAnimation* m_attackAnimation;
		yunuGI::IAnimation* m_paralysisAnimation;
		yunuGI::IAnimation* m_deathAnimation;
	};

	Timer* knockBackTimer;
	Dotween* dotween;
	yunutyEngine::graphics::Animator* m_animatorComponent;
	NavigationAgent* m_navAgentComponent;

	//Vector3d startPosition;
private:
	FSM<UnitState> unitFSM{UnitState::Idle};

	UnitType m_unitType;
	UnitSide m_unitSide;
	AttackType m_attackType;

	std::string m_fbxName;
	float m_maxHealthPoint;
	float m_currentHealthPoint;
	float m_autoAttackDamage;
	float m_beforeBlindDamage;
	float m_initialAutoAttackDamage;
	float m_maxAutoAttackDamage = 10.0f;
	float m_minAttackDelay = 0.3f;
	
	float m_finalAttackDamage;
	float m_finalHitDamage;

	int m_randcriticalHitNumber;

	float m_defensePoint;
	float m_dodgeProbability;
	float m_criticalDamageDecreaseMultiplier;
	float m_criticalHitMultiplier;

	int m_criticalHitProbability;

	float m_speed;
	float m_bulletSpeed;

	float chaseUpdateDelay;
	// 유닛의 현재 회전값을 동경각으로 나타냅니다. 유닛은 currentRotation이 0도일때 동쪽(양의 X 방향), 90도일때 북쪽(양의 z 방향)을 향합니다.
	float currentRotation{270};
	// 유닛이 바라봐야 하는 회전값을 동경각으로 나타냅니다. 유닛은 회전속도에 따라 회전값을 desiredRotation과 일치하게 바꿉니다.
	float desiredRotation{270};
	float rotationSpeed{ 500 };
	float rotateTime = 0.2f;

	// 지금 수행중인 명령
	UnitState currentOrder = UnitState::Idle;

	float m_idDistance;
	float m_atkDistance;

	float idleToChaseDelay = 1.5f;
	float idleElapsed;

	float moveFunctionElapsed;
	float moveFunctionCallDelay = 0.1f;

	float chaseFunctionElapsed;
	float chaseFunctionCallDelay = 0.1f;

	float attackStartElapsed = 0.0f;

	float attackStartDelay = 0.3f;
	float attackFunctionElapsed;
	float attackFunctionCallDelay;
	float increasedAttackSpeed{ 1.0f };
	int attackTimingFrame;
	int attackAnimationFrameCheckNumber = 0;
	bool isAttackAnimationOperating = false;
	bool isAttacked = false;
	bool isAttackStarted = false;
	float m_attackOffset;


	float deathFunctionElapsed;
	float deathAnimationDelay = 1.5f;

	float qSkillFunctionStartElapsed;
	//float qSkillFunctionStartedElapsed;
	//float qSkillStartDelay = 1.0f;
	float qSkillAnimationDuration = 5.0f;
	bool isJustHitByQSkill = false;

	float animationLerpDuration = 1.0f;
	float animationTransitionSpeed = 3.0f;

	bool isSkillStarted;

	bool isAttackMoving;

	UnitType playerSerialNumber;

	std::set<Unit*> m_opponentObjectSet;		// idradius에 닿는 모든 상대 유닛을 담아줍니다.
	std::set<Unit*> m_recognizedThisSet;		// 현재 이 유닛을 적군으로 인식한 다른 유닛들의 리스트.

	std::set<Unit*> m_attackingThisUnitSet;		// 현재 이 유닛을 공격하고 있는 유닛들
	int m_maxAggroNumber;

	Unit* m_currentTargetUnit;		// Attack이나 Chase 때 사용할 적군  오브젝트
	Unit* m_previousTargetUnit;		
	Unit* tauntingThisUnit;				// 현재 this 유닛이 도발당한 주체.
	Vector3d m_previousMovePosition;						
	Vector3d m_currentMovePosition;							// 현재 상대의 위치

	Vector3d m_currentSkillPosition;

	yunutyEngine::graphics::StaticMeshRenderer* m_staticMeshRenderer;

	SkillPreviewMesh m_qSkillPreviewType;
	SkillPreviewMesh m_wSkillPreviewType;

	Unit* m_followingTargetUnit;
	float m_followEngageDinstance{ 4.0f };			// 이 수치만큼 거리가 벌어지면 따라간다.
	float m_stopFollowDinstance{ 2.0f };			// 이 수치만큼 거리가 좁혀지면 멈춘다.
	bool isFollowing{ false };
private:
	/// 유닛이 속해있는 field
	NavigationField* m_unitNavField;

	/// 현재 스킬 활성화 시 적용되는 스킬 적용 위치
	Vector3d m_skillPosition;
	SkillEnum m_currentSelectedSkill;

private:
	void IdleEngage();
	void MoveEngage();
	void OffsetMoveEngage();
	void AttackMoveEngage();
	void AttackEngage();
	void ChaseEngage();
	void SkillEngage();
	void ParalysisEngage();
	void DeathEngage();

	void IdleUpdate();
	void MoveUpdate();
	void OffsetMoveUpdate();
	void AttackMoveUpdate();
	void ChaseUpdate();
	void AttackUpdate();
	void SkillUpdate();
	void DeathUpdate();
	
	void CheckCurrentAnimation(yunuGI::IAnimation* currentStateAnimation);

	
	void ReportUnitDeath();												// this 유닛이 죽었다는 정보를 전달
	void IdentifiedOpponentDeath(Unit* p_unit);		// 상대 유닛이 죽었을 경우 처리할 내용을 담은 함수

	void DetermineHitDamage(float p_onceCalculatedDmg);					// 피격유닛이 받는 최종 데미지 계산

	void RotateUnit(Vector3d endPosition);
public:
	BaseUnitAnimationStruct unitAnimations;

	virtual void Start() override;
	virtual void Update() override;

	void StopMove();
	UnitType GetUnitType() const;
	UnitSide GetUnitSide() const;
	void SetUnitType(UnitType type);
	void SetUnitSide(UnitSide side);
	void SetUnitMaxHp(int p_Hp);
	void SetUnitAp(int p_Ap);
	void SetIdRadius(float radius);
	void SetAtkRadius(float radius);
	void SetUnitSpeed(float speed);
	void SetAttackDelay(float p_delay);
	void SetAttackTimingFrame(int p_frame);
	void SetPlayerSerialNumber(UnitType serialNum);
	void SetSkillDuration(float p_duration);
	void SetSkillPreviewType(SkillPreviewMesh p_qskill, SkillPreviewMesh p_wskill);
	void SetMaxAggroNumber(int p_num);
	void SetFbxName(std::string p_string);
	void SetAttackOffset(float p_offset);

	void SetCurrentOrderMove();
	void SetCurrentOrderAttackMove();

	float GetAttackOffset() const;

	void OrderMove(Vector3d position);
	void OrderAttackMove(Vector3d position);
	void OrderSkill(SkillEnum p_skillNum, Vector3d position);
	void OrderSkill(SkillEnum p_skillNum);

	void DetermineCurrentTargetObject();
	void AddToOpponentObjectList(Unit* p_unit);
	void DeleteFromOpponentObjectList(Unit* p_unit);

	void SetNavField(NavigationField* p_navField);
	NavigationField* GetNavField() const;

	void MakeUnitPushedState(bool p_isCrushed);
	void MakeUnitParalysisState();
	void MakeUnitStateIdle();

	bool GetJustCrushedState() const;
	bool IsUnitDead() const;

	UnitType GetPlayerSerialNumber() const;
	UnitState GetCurrentUnitState() const;
	SkillPreviewMesh GetSkillPreviewType(Unit::SkillEnum p_currentSkillType) const;
	std::string GetUnitFbxName() const;

	float DetermineAttackDamage(float p_damage);			// 공격유닛이 피격유닛에게 전달하는 데미지 계산.

	void SetStaticMeshComponent(yunutyEngine::graphics::StaticMeshRenderer* p_stcMesh);

	void ChangeCurrentOpponentUnitForced(Unit* p_unit);
	void DeleteTauntingUnit();

	void SetUnitStateToDeath();
	void SetUnitStateToSkill();

public:
	int GetUnitDamage() const;
	void Damaged(Unit* opponentUnit, float opponentAp);	// 데미지 입었을 경우 추적하는 로직 포함
	void Damaged(float dmg);										// 추적받지 않는 데미지
	void Heal(float healingPoint);										
	
	void IncreaseAttackPower(float p_attackPowerIncrease);
	void IncreaseAttackSpeed(float p_attackSpeedIncrease);

	void SetUnitDamageToZero(bool p_bool);
	void MultipleUnitSpeed(float p_mul);

	void ResetUnitMembers();

	std::function<void()> returnToPoolFunction{ nullptr };
	DummyComponent* m_dummyCom;
	int stageNumber;

	std::vector<std::function<void()>> OnCreated;
	std::vector<std::function<void()>> OnDeath;
	bool isJustCreated{ false };


	friend RobinSkillDevelopmentSystem;
	friend UnitProductor;
};

