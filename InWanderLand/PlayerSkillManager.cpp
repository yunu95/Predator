#include "PlayerSkillManager.h"
#include "Unit.h"
#include "InWanderLand.h"
#include "GlobalConstant.h"

void PlayerSkillManager::Start()
{
    m_maxSkillUsageGauge = 100;

    AddSkillGauge(100);
    //m_currentSkillUsageGauge = 100;

	warriorSkillOneCost = application::GlobalConstant::GetSingletonInstance().pod.robinQSkillCost;
	warriorSkillTwoCost = application::GlobalConstant::GetSingletonInstance().pod.robinESkillCost;

	magicianSkillOneCost = application::GlobalConstant::GetSingletonInstance().pod.ursulaQSkillCost;
	magicianSkillTwoCost = application::GlobalConstant::GetSingletonInstance().pod.ursulaESkillCost;

	healerSkillOneCost = application::GlobalConstant::GetSingletonInstance().pod.hanselQSkillCost;
	healerSkillTwoCost = application::GlobalConstant::GetSingletonInstance().pod.hanselESkillCost;

	m_skillGaugeRecoveryPerSecond = application::GlobalConstant::GetSingletonInstance().pod.skillGaugeRecoveryPerSecond;

	costPerSkillMap[std::make_pair(Unit::UnitType::Warrior, Unit::SkillEnum::Q)] = warriorSkillOneCost;
	costPerSkillMap[std::make_pair(Unit::UnitType::Warrior, Unit::SkillEnum::W)] = warriorSkillTwoCost;

    costPerSkillMap[std::make_pair(Unit::UnitType::Magician, Unit::SkillEnum::Q)] = magicianSkillOneCost;
    costPerSkillMap[std::make_pair(Unit::UnitType::Magician, Unit::SkillEnum::W)] = magicianSkillTwoCost;

    costPerSkillMap[std::make_pair(Unit::UnitType::Healer, Unit::SkillEnum::Q)] = healerSkillOneCost;
    costPerSkillMap[std::make_pair(Unit::UnitType::Healer, Unit::SkillEnum::W)] = healerSkillTwoCost;

    isSingletonComponent = true;

	m_skillGaugeRecoveryElapsed = 0.0f;
}

void PlayerSkillManager::Update()
{
	AddSkillGauge(m_skillGaugeRecoveryPerSecond * Time::GetDeltaTime());
}

void PlayerSkillManager::PlayFunction()
{
    this->SetActive(true);
    if (isOncePaused)
    {
        Start();
    }
}

void PlayerSkillManager::StopFunction()
{
    costPerSkillMap.clear();
    this->SetActive(false);
}

bool PlayerSkillManager::IsSkillGaugeEnoughToBeUsed(Unit::UnitType p_unitType, Unit::SkillEnum p_skillEnum)
{
    return (m_currentSkillUsageGauge - costPerSkillMap.find({ p_unitType, p_skillEnum })->second >= 0);
}

void PlayerSkillManager::ReportSkillUsed(Unit::UnitType p_unitType, Unit::SkillEnum p_skillEnum)
{
    AddSkillGauge(-1 * costPerSkillMap.find({ p_unitType, p_skillEnum })->second);
}

void PlayerSkillManager::AddSkillGauge(int p_addedGauge)
{
    /// 이곳에서 스킬 게이지의 변화가 일어납니다.
    m_currentSkillUsageGauge += p_addedGauge;

    if (m_currentSkillUsageGauge > m_maxSkillUsageGauge)
        m_currentSkillUsageGauge = m_maxSkillUsageGauge;
    else if (m_currentSkillUsageGauge < 0)
        m_currentSkillUsageGauge = 0;

    ReportSkillGaugeChanged();
}

void PlayerSkillManager::ReportSkillGaugeChanged()
{
    /// m_currentSkillUsageGauge가 변경될 때마다 호출되는 함수입니다!
    UIManager::Instance().GetUIElementByEnum(UIEnumID::ManaFill)->adjuster->SetTargetFloat(1 - m_currentSkillUsageGauge / static_cast<float>(m_maxSkillUsageGauge));
    UIManager::Instance().GetUIElementByEnum(UIEnumID::Mana_Text_MaxMP)->SetNumber(m_maxSkillUsageGauge);
    UIManager::Instance().GetUIElementByEnum(UIEnumID::Mana_Text_CurrentMP)->SetNumber(m_currentSkillUsageGauge);

}
