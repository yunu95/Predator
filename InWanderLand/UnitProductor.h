#pragma once
#include "YunutyEngine.h"
#include "Unit.h"
#include "SkillPreviewSystem.h"
#include "PermanentObservee.h"

namespace application
{
	namespace editor
	{
		struct POD_Unit_TemplateData;
	}
}


static float constexpr UNIT_LENGTH = 2.0f;

/// <summary>
/// UnitFactory에서의 유닛 생산을 좀 더 효율적으로 하기 위한 클래스.3
/// </summary>
class UnitProductor : public Component, public PermanentObservee
{
protected:
	// 임시. 모델링 파일로 대체해주기
	GameObject* m_unitGameObject;
	Unit* m_unitComponent;

	Unit::BaseUnitAnimationStruct m_baseUnitAnimations;

	NavigationField* m_navField;

	float m_elapsed;
	float m_duration;

	int m_currentIndex = 0;
	float m_previousDelay;

	std::string m_unitFbxName;

public:
	string m_objectName;
	Unit::UnitType m_unitType;
	Unit::UnitSide m_unitSide;

	SkillPreviewMesh qSkillPreviewType;
	SkillPreviewMesh wSkillPreviewType;

	float m_unitScaleMultipler;
	float m_healthPoint;
	int m_manaPoint;

	float m_autoAttackDamage;
	int m_criticalHitProbability;				// 치명타 확률
	float m_criticalHitMultiplier;				// 공격 시 치명타 피해량

	int m_maxAggroNumber;

	/// Decreasing Damage Elements
	int m_defensePoint;
	int m_dodgeProbability;					// 회피율
	float m_criticalDamageDecreaseMultiplier;	// 피격 시 치명타 피해 감소 배율

	float m_idRadius;
	float m_atkRadius;
	float m_unitSpeed;

	float m_groupMoveRadius;

	float m_attackDelay;
	int m_attackTimingFrame;
	float m_attackOffset;

	float	m_skillOneEngageDelay;
	int		m_skillOneTimingFrame;
	float	m_skillTwoEngageDelay;
	int		m_skillTwoTimingFrame;
	float	m_skillThreeEngageDelay;
	int		m_skillThreeTimingFrame;
	float	m_skillFourEngageDelay;
	int		m_skillFourTimingFrame;

	virtual Unit* CreateUnit(Vector3d startPos) = 0;

	void SetUnitComponentMembers();
	void AddRangeSystemComponent() const;
	void AddColliderComponent() const;
	void AddNavigationComponent();
	void AddDotweenComponent() const;
	void SetUnitAnimationFunction();

	virtual bool SelectUnitProductorByFbxName(std::string p_name);

	virtual std::string GetUnitFBXName() const;

	virtual void SetUnitData() = 0;

	virtual void SetPlayerRelatedComponents();
	virtual void MappingUnitData(application::editor::POD_Unit_TemplateData p_podData);

	virtual void Update() override;
	virtual void Start() override;

	virtual void PlayFunction() override final;
	virtual void StopFunction() override final;
};

