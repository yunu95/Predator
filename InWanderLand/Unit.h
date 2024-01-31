#pragma once
#include "YunutyEngine.h"
#include "FSM.h"
#include "Timer.h"
#include "Dotween.h"
#include <list>

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
		StateEnd
	};

	enum class AttackType
	{
		Melee,
		Ranged
	};

	enum class UnitType
	{
		Warrior,
		Magician,
		Healer,
		MeleeEnemy,
		Boss
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
		E
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

protected:
	FSM<UnitState> unitFSM{UnitState::Idle};

	UnitType m_unitType;
	UnitSide m_unitSide;
	AttackType m_attackType;

	float m_healthPoint;
	float m_autoAttackDamage;
	float m_beforeBlindDamage;
	float m_initialAutoAttackDamage;
	float m_maxAutoAttackDamage = 10.0f;
	float m_minAttackDelay = 0.3f;
	
	float m_finalAttackDamage;
	float m_finalHitDamage;

	float m_defensePoint;
	float m_dodgeProbability;
	float m_criticalDamageDecreaseMultiplier;

	int m_criticalHitProbability;
	int m_randcriticalHitNumber;

	float m_criticalHitMultiplier;

	float m_speed;
	float m_bulletSpeed;

	float chaseUpdateDelay;
	// 유닛의 현재 회전값을 동경각으로 나타냅니다. 유닛은 currentRotation이 0도일때 동쪽(양의 X 방향), 90도일때 북쪽(양의 z 방향)을 향합니다.
	float currentRotation{270};
	// 유닛이 바라봐야 하는 회전값을 동경각으로 나타냅니다. 유닛은 회전속도에 따라 회전값을 desiredRotation과 일치하게 바꿉니다.
	float desiredRotation{270};
	float rotationSpeed{ 500 };
	float rotationTime = 0.2f;

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
	int attackAnimationFrameCheckNumber = 0;
	bool isAttackAnimationOperating = false;
	bool isAttackStarted = false;

	float deathFunctionElapsed;
	float deathAnimationDelay = 1.5f;

	float qSkillFunctionStartElapsed;
	//float qSkillFunctionStartedElapsed;
	//float qSkillStartDelay = 1.0f;
	float qSkillAnimationDuration = 1.0f;
	bool isJustHitByQSkill = false;

	float animationLerpDuration = 1.0f;
	float animationTransitionSpeed = 3.0f;

	bool isSkillStarted;

	bool isAttackMoving;

	int playerSerialNumber;

	std::list<yunutyEngine::GameObject*> m_opponentObjectList;		
	std::list<Unit*> m_recognizedThisList;	// 현재 이 오브젝트를 적군으로 인식한 다른 유닛들의 리스트.

	yunutyEngine::GameObject* m_currentTargetObject;		// Attack이나 Chase 때 사용할 적군  오브젝트
	Vector3d m_currentMovePosition;							// 현재 상대의 위치

	Vector3d m_currentSkillPosition;

protected:
	/// 유닛이 속해있는 field
	NavigationField* m_unitNavField;

	/// 현재 스킬 활성화 시 적용되는 스킬 적용 위치
	Vector3d m_skillPosition;
	SkillEnum m_currentSelectedSkill;

protected:
	void IdleEngage();
	void MoveEngage();
	void AttackMoveEngage();
	void AttackEngage();
	void ChaseEngage();
	void SkillEngage();
	void ParalysisEngage();
	void DeathEngage();

	void IdleUpdate();
	void MoveUpdate();
	void AttackMoveUpdate();
	void ChaseUpdate();
	void AttackUpdate();
	void SkillUpdate();
	void DeathUpdate();
	
	void CheckCurrentAnimation(yunuGI::IAnimation* currentStateAnimation);

	void StopMove();
	
	void ReportUnitDeath();												// this 유닛이 죽었다는 정보를 전달
	void IdentifiedOpponentDeath(yunutyEngine::GameObject* obj);		// 상대 유닛이 죽었을 경우 처리할 내용을 담은 함수

	void DetermineHitDamage(float p_onceCalculatedDmg);					// 피격유닛이 받는 최종 데미지 계산

public:
	BaseUnitAnimationStruct unitAnimations;

	virtual void Start() override;
	virtual void Update() override;

	UnitType GetUnitType() const;
	UnitSide GetUnitSide() const;
	void SetUnitType(UnitType type);
	void SetUnitSide(UnitSide side);
	void SetUnitHp(int p_Hp);
	void SetUnitAp(int p_Ap);
	void SetIdRadius(float radius);
	void SetAtkRadius(float radius);
	void SetUnitSpeed(float speed);
	void SetAttackDelay(float p_delay);
	void SetPlayerSerialNumber();
	void SetSkillDuration(float p_duration);

	void SetCurrentOrderMove();
	void SetCurrentOrderAttackMove();

	void OrderMove(Vector3d position);
	void OrderAttackMove(Vector3d position);
	void OrderSkill(SkillEnum p_skillNum, Vector3d position);
	void OrderSkill(SkillEnum p_skillNum);

	void DetermineCurrentTargetObject();
	void AddToOpponentObjectList(yunutyEngine::GameObject* obj);
	void DeleteFromOpponentObjectList(yunutyEngine::GameObject* obj);

	void AddToRecognizeList(Unit* p_unit);
	void DeleteFromRecognizeList(Unit* p_unit);

	void SetNavField(NavigationField* p_navField);
	NavigationField* GetNavField() const;

	void MakeUnitPushedState(bool p_isCrushed);
	void MakeUnitParalysisState();
	void MakeUnitParalysisEnd();

	bool GetJustCrushedState() const;
	bool IsUnitDead() const;

	int GetPlayerSerialNumber() const;


	float DetermineAttackDamage(float p_damage);			// 공격유닛이 피격유닛에게 전달하는 데미지 계산.

public:
	/// <summary>
	/// 유닛의 스탯을 조작하는 함수.
	/// </summary>
	/// <returns></returns>
	int GetUnitDamage() const;
	void Damaged(GameObject* opponentObject, float opponentAp);	// 데미지 입었을 경우 추적하는 로직 포함
	void Damaged(float dmg);										// 추적받지 않는 데미지
	void Heal(float healingPoint);										
	
	void IncreaseAttackPower(float p_attackPowerIncrease);
	void IncreaseAttackSpeed(float p_attackSpeedIncrease);

	void SetUnitDamageToZero(bool p_bool);
	void MultipleUnitSpeed(float p_mul);
};

