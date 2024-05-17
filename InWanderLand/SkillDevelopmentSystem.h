#pragma once
#include "YunutyEngine.h"
/// <summary>
/// 플레이어 유닛들의 스킬 강화 시스템.
/// UIButton Component를 배치 후 시스템에 등록한다면 
/// 버튼 클릭시 호출되는 함수를 다른 컴포넌트들과(ex.unit, skillsystem) 연결시켜준다.
/// </summary>

class UIButton;
class Unit;
class PlayerSkillSystem;

class SkillDevelopmentSystem : virtual public Component
{
protected:
	Unit* m_targetUnit;
	PlayerSkillSystem* m_ownerSkillSystem;

	int m_skillPoint;
	int m_usedPoint;

	std::vector<UIButton*> m_topNodeSkillButton;
	std::vector<UIButton*> m_middleNodeSkillButton;
	std::vector<UIButton*> m_buttomNodeSkillButton;

public:
	virtual void SetOwnerUnit(Unit* p_unit);
	virtual void AddTopLayerButton(UIButton* p_btn);
	virtual void AddMiddleLayerButton(UIButton* p_btn);
	virtual void AddLowLayerButton(UIButton* p_btn);
	virtual void SetSkillSystemComponent(PlayerSkillSystem* p_skillSys);

};

