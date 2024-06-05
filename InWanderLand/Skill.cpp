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
		case SkillType::EnemyImpale:
		{
			return ::SkillPodFieldPreEncoding<SkillType::EnemyImpale>(data);
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
		case SkillType::EnemyImpale:
		{
			return ::SkillPodFieldPostEncoding<SkillType::EnemyImpale>(data);
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
		case SkillType::EnemyImpale:
		{
			return ::SkillPodFieldPreDecoding<SkillType::EnemyImpale>(data);
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
		case SkillType::EnemyImpale:
		{
			return ::SkillPodFieldPostDecoding<SkillType::EnemyImpale>(data);
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
		case SkillType::EnemyImpale:
		{
			application::editor::imgui::data::DrawDataField(EnemyImpaleSkill::pod);
			break;
		}
		default:
			break;
	}
}
