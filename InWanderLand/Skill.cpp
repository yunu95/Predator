#include "Skill.h"

#include "Storable.h"
#include "imgui_ModuleData.h"
#include "SkillList.h"

bool Skill::SkillPodFieldPreEncoding(SkillType::Enum type, json& data)
{
	switch (type)
	{
		case SkillType::ROBIN_Q:
		{
			return ::SkillPodFieldPreEncoding<SkillType::ROBIN_Q>(data);
		}
		case SkillType::ROBIN_W:
		{
			return ::SkillPodFieldPreEncoding<SkillType::ROBIN_W>(data);
		}
		case SkillType::URSULA_Q:
		{
			return ::SkillPodFieldPreEncoding<SkillType::URSULA_Q>(data);
		}
		case SkillType::URSULA_W:
		{
			return ::SkillPodFieldPreEncoding<SkillType::URSULA_W>(data);
		}
		case SkillType::HANSEL_Q:
		{
			return ::SkillPodFieldPreEncoding<SkillType::HANSEL_Q>(data);
		}
		case SkillType::HANSEL_W:
		{
			return ::SkillPodFieldPreEncoding<SkillType::HANSEL_W>(data);
		}
		case SkillType::EnemySpin:
		{
			return ::SkillPodFieldPreEncoding<SkillType::EnemySpin>(data);
		}
		case SkillType::EnemyImpale:
		{
			return ::SkillPodFieldPreEncoding<SkillType::EnemyImpale>(data);
		}
		case SkillType::BossSkill_One:
		{
			return ::SkillPodFieldPreEncoding<SkillType::BossSkill_One>(data);
		}
		case SkillType::BossSkill_Two:
		{
			return ::SkillPodFieldPreEncoding<SkillType::BossSkill_Two>(data);
		}
		case SkillType::BossSkill_Three:
		{
			return ::SkillPodFieldPreEncoding<SkillType::BossSkill_Three>(data);
		}
		case SkillType::BossSkill_Four:
		{
			return ::SkillPodFieldPreEncoding<SkillType::BossSkill_Four>(data);
		}
		case SkillType::Passive_Robin_Bleeding:
		{
			return ::SkillPodFieldPreEncoding<SkillType::Passive_Robin_Bleeding>(data);
		}
		case SkillType::Passive_Ursula_Speed:
		{
			return ::SkillPodFieldPreEncoding<SkillType::Passive_Ursula_Speed>(data);
		}
		case SkillType::Passive_Hansel_Heal:
		{
			return ::SkillPodFieldPreEncoding<SkillType::Passive_Hansel_Heal>(data);
		}
		default:
			break;
	}

	return false;
}

bool Skill::SkillPodFieldPostEncoding(SkillType::Enum type, json& data)
{
	switch (type)
	{
		case SkillType::ROBIN_Q:
		{
			return ::SkillPodFieldPostEncoding<SkillType::ROBIN_Q>(data);
		}
		case SkillType::ROBIN_W:
		{
			return ::SkillPodFieldPostEncoding<SkillType::ROBIN_W>(data);
		}
		case SkillType::URSULA_Q:
		{
			return ::SkillPodFieldPostEncoding<SkillType::URSULA_Q>(data);
		}
		case SkillType::URSULA_W:
		{
			return ::SkillPodFieldPostEncoding<SkillType::URSULA_W>(data);
		}
		case SkillType::HANSEL_Q:
		{
			return ::SkillPodFieldPostEncoding<SkillType::HANSEL_Q>(data);
		}
		case SkillType::HANSEL_W:
		{
			return ::SkillPodFieldPostEncoding<SkillType::HANSEL_W>(data);
		}
		case SkillType::EnemySpin:
		{
			return ::SkillPodFieldPostEncoding<SkillType::EnemySpin>(data);
		}
		case SkillType::EnemyImpale:
		{
			return ::SkillPodFieldPostEncoding<SkillType::EnemyImpale>(data);
		}
		case SkillType::BossSkill_One:
		{
			return ::SkillPodFieldPostEncoding<SkillType::BossSkill_One>(data);
		}
		case SkillType::BossSkill_Two:
		{
			return ::SkillPodFieldPostEncoding<SkillType::BossSkill_Two>(data);
		}
		case SkillType::BossSkill_Three:
		{
			return ::SkillPodFieldPostEncoding<SkillType::BossSkill_Three>(data);
		}
		case SkillType::BossSkill_Four:
		{
			return ::SkillPodFieldPostEncoding<SkillType::BossSkill_Four>(data);
		}
		case SkillType::Passive_Robin_Bleeding:
		{
			return ::SkillPodFieldPostEncoding<SkillType::Passive_Robin_Bleeding>(data);
		}
		case SkillType::Passive_Ursula_Speed:
		{
			return ::SkillPodFieldPostEncoding<SkillType::Passive_Ursula_Speed>(data);
		}
		case SkillType::Passive_Hansel_Heal:
		{
			return ::SkillPodFieldPostEncoding<SkillType::Passive_Hansel_Heal>(data);
		}
		default:
			break;
	}

	return false;
}

bool Skill::SkillPodFieldPreDecoding(SkillType::Enum type, const json& data)
{
	switch (type)
	{
		case SkillType::ROBIN_Q:
		{
			return ::SkillPodFieldPreDecoding<SkillType::ROBIN_Q>(data);
		}
		case SkillType::ROBIN_W:
		{
			return ::SkillPodFieldPreDecoding<SkillType::ROBIN_W>(data);
		}
		case SkillType::URSULA_Q:
		{
			return ::SkillPodFieldPreDecoding<SkillType::URSULA_Q>(data);
		}
		case SkillType::URSULA_W:
		{
			return ::SkillPodFieldPreDecoding<SkillType::URSULA_W>(data);
		}
		case SkillType::HANSEL_Q:
		{
			return ::SkillPodFieldPreDecoding<SkillType::HANSEL_Q>(data);
		}
		case SkillType::HANSEL_W:
		{
			return ::SkillPodFieldPreDecoding<SkillType::HANSEL_W>(data);
		}
		case SkillType::EnemySpin:
		{
			return ::SkillPodFieldPreDecoding<SkillType::EnemySpin>(data);
		}
		case SkillType::EnemyImpale:
		{
			return ::SkillPodFieldPreDecoding<SkillType::EnemyImpale>(data);
		}
		case SkillType::BossSkill_One:
		{
			return ::SkillPodFieldPreDecoding<SkillType::BossSkill_One>(data);
		}
		case SkillType::BossSkill_Two:
		{
			return ::SkillPodFieldPreDecoding<SkillType::BossSkill_Two>(data);
		}
		case SkillType::BossSkill_Three:
		{
			return ::SkillPodFieldPreDecoding<SkillType::BossSkill_Three>(data);
		}
		case SkillType::BossSkill_Four:
		{
			return ::SkillPodFieldPreDecoding<SkillType::BossSkill_Four>(data);
		}
		case SkillType::Passive_Robin_Bleeding:
		{
			return ::SkillPodFieldPreDecoding<SkillType::Passive_Robin_Bleeding>(data);
		}
		case SkillType::Passive_Ursula_Speed:
		{
			return ::SkillPodFieldPreDecoding<SkillType::Passive_Ursula_Speed>(data);
		}
		case SkillType::Passive_Hansel_Heal:
		{
			return ::SkillPodFieldPreDecoding<SkillType::Passive_Hansel_Heal>(data);
		}
		default:
			break;
	}

	return false;
}

bool Skill::SkillPodFieldPostDecoding(SkillType::Enum type, const json& data)
{
	switch (type)
	{
		case SkillType::ROBIN_Q:
		{
			return ::SkillPodFieldPostDecoding<SkillType::ROBIN_Q>(data);
		}
		case SkillType::ROBIN_W:
		{
			return ::SkillPodFieldPostDecoding<SkillType::ROBIN_W>(data);
		}
		case SkillType::URSULA_Q:
		{
			return ::SkillPodFieldPostDecoding<SkillType::URSULA_Q>(data);
		}
		case SkillType::URSULA_W:
		{
			return ::SkillPodFieldPostDecoding<SkillType::URSULA_W>(data);
		}
		case SkillType::HANSEL_Q:
		{
			return ::SkillPodFieldPostDecoding<SkillType::HANSEL_Q>(data);
		}
		case SkillType::HANSEL_W:
		{
			return ::SkillPodFieldPostDecoding<SkillType::HANSEL_W>(data);
		}
		case SkillType::EnemySpin:
		{
			return ::SkillPodFieldPostDecoding<SkillType::EnemySpin>(data);
		}
		case SkillType::EnemyImpale:
		{
			return ::SkillPodFieldPostDecoding<SkillType::EnemyImpale>(data);
		}
		case SkillType::BossSkill_One:
		{
			return ::SkillPodFieldPostDecoding<SkillType::BossSkill_One>(data);
		}
		case SkillType::BossSkill_Two:
		{
			return ::SkillPodFieldPostDecoding<SkillType::BossSkill_Two>(data);
		}
		case SkillType::BossSkill_Three:
		{
			return ::SkillPodFieldPostDecoding<SkillType::BossSkill_Three>(data);
		}
		case SkillType::BossSkill_Four:
		{
			return ::SkillPodFieldPostDecoding<SkillType::BossSkill_Four>(data);
		}
		case SkillType::Passive_Robin_Bleeding:
		{
			return ::SkillPodFieldPostDecoding<SkillType::Passive_Robin_Bleeding>(data);
		}
		case SkillType::Passive_Ursula_Speed:
		{
			return ::SkillPodFieldPostDecoding<SkillType::Passive_Ursula_Speed>(data);
		}
		case SkillType::Passive_Hansel_Heal:
		{
			return ::SkillPodFieldPostDecoding<SkillType::Passive_Hansel_Heal>(data);
		}
		default:
			break;
	}

	return false;
}

void Skill::DrawSkillPod(SkillType::Enum type)
{
	switch (type)
	{
		case SkillType::ROBIN_Q:
		{
			application::editor::imgui::data::DrawDataField(RobinChargeSkill::pod);
			break;
		}
		case SkillType::ROBIN_W:
		{
			application::editor::imgui::data::DrawDataField(RobinTauntSkill::pod);
			break;
		}
		case SkillType::URSULA_Q:
		{
			application::editor::imgui::data::DrawDataField(UrsulaBlindSkill::pod);
			break;
		}
		case SkillType::URSULA_W:
		{
			application::editor::imgui::data::DrawDataField(UrsulaParalysisSkill::pod);
			break;
		}
		case SkillType::HANSEL_Q:
		{
			application::editor::imgui::data::DrawDataField(HanselChargeSkill::pod);
			break;
		}
		case SkillType::HANSEL_W:
		{
			application::editor::imgui::data::DrawDataField(HanselProjectileSkill::pod);
			break;
		}
		case SkillType::EnemySpin:
		{
			application::editor::imgui::data::DrawDataField(EnemySpinAttackSkill::pod);
			break;
		}
		case SkillType::EnemyImpale:
		{
			application::editor::imgui::data::DrawDataField(EnemyImpaleSkill::pod);
			break;
		}
		case SkillType::BossSkill_One:
		{
			application::editor::imgui::data::DrawDataField(BossSpinAttackSkill::pod);
			break;
		}
		case SkillType::BossSkill_Two:
		{
			application::editor::imgui::data::DrawDataField(BossImpaleSkill::pod);
			break;
		}
		case SkillType::BossSkill_Three:
		{
			application::editor::imgui::data::DrawDataField(BossSummonMobSkill::pod);
			break;
		}
		case SkillType::BossSkill_Four:
		{
			application::editor::imgui::data::DrawDataField(BossSummonChessSkill::pod);
			break;
		}
		case SkillType::Passive_Robin_Bleeding:
		{
			application::editor::imgui::data::DrawDataField(PassiveRobinBleed::pod);
			break;
		}
		case SkillType::Passive_Ursula_Speed:
		{
			application::editor::imgui::data::DrawDataField(PassiveUrsula::pod);
			break;
		}
		case SkillType::Passive_Hansel_Heal:
		{
			application::editor::imgui::data::DrawDataField(PassiveHanselHeal::pod);
			break;
		}
		default:
			break;
	}
}
