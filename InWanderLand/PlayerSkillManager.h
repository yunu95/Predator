#pragma once
#include "YunutyEngine.h"
#include "ContentsObservee.h"
#include "Unit.h"
#include <unordered_map>

/// <summary>
/// 플레이어 스킬 사용 시 소모되는 게이지에 대한 정보를 공유합니다.
/// </summary>

namespace std {
	template <>
	struct hash<std::pair<Unit::UnitType, Unit::SkillEnum>> {
		size_t operator()(const std::pair<Unit::UnitType, Unit::SkillEnum>& p) const {
			auto hash1 = std::hash<int>{}(static_cast<int>(p.first));
			auto hash2 = std::hash<int>{}(static_cast<int>(p.second));
			return hash1 ^ hash2; // 간단한 해시 결합 기법 사용
		}
	};
}

class PlayerSkillManager : public Component, public SingletonComponent<PlayerSkillManager>, public ContentsObservee
{
private:
	int m_maxSkillUsageGauge;
	int m_currentSkillUsageGauge;


	int warriorSkillOneCost;
	int warriorSkillTwoCost;
	int magicianSkillOneCost;
	int magicianSkillTwoCost;
	int healerSkillOneCost;
	int healerSkillTwoCost;

	std::unordered_map<std::pair<Unit::UnitType, Unit::SkillEnum>, int> costPerSkillMap;

	float m_skillGaugeRefillDuration;
	float m_skillGaugeRefillElapsed;

	void ReportSkillGaugeChanged();

public:
	virtual void Start() override;
	virtual void Update() override;
	virtual void PlayFunction() override;
	virtual void StopFunction() override;

	bool IsSkillGaugeEnoughToBeUsed(Unit::UnitType p_unitType, Unit::SkillEnum p_skillEnum);

	void ReportSkillUsed(Unit::UnitType p_unitType, Unit::SkillEnum p_skillEnum);

	void AddSkillGauge(int p_addedGauge);

};

