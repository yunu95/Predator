#include "LeftFrame.h"

#include "InWanderLand.h"
#include "UnitData.h"
#include "DebugMeshes.h"
#include "Unit.h"
#include "TacticModeSystem.h"
#include "YunutyWaitForSeconds.h"
#include "SFXManager.h"
#include "BossSummonMobSkill.h"
#include "GlobalConstant.h"
#include "UnitPool.h"
#include "Unit_TemplateData.h"
#include "TemplateDataManager.h"

namespace BossSummon
{
	application::editor::Unit_TemplateData* LeftFrame::meleeUnitMold = nullptr;
	application::editor::Unit_TemplateData* LeftFrame::projectileUnitMold = nullptr;

	LeftFrame::~LeftFrame()
	{
		OnBossDie();
		meleeUnitMold = nullptr;
		projectileUnitMold = nullptr;
		BossSummonMobSkill::SetLeftFrame(nullptr);
	}

	void LeftFrame::Init(application::editor::ITemplateData* templateData)
	{
		for (auto each : GetGameObject()->GetChildren())
		{
			auto renderer = each->GetComponent<graphics::SkinnedMesh>();
			if (renderer)
			{
				mesh = each;
				break;
			}
		}

		mesh->SetSelfActive(false);
	}

	void LeftFrame::OnSummon()
	{
		for (auto each : application::editor::TemplateDataManager::GetSingletonInstance().GetDataList(application::editor::DataType::UnitData))
		{
			auto td = static_cast<application::editor::Unit_TemplateData*>(each);
			if (td->pod.skinnedFBXName == "SKM_Monster1" && td->pod.unitControllerType.enumValue != UnitControllerType::MELEE_ELITE)
			{
				meleeUnitMold = td;
			}
			else if (td->pod.skinnedFBXName == "SKM_Monster2" && td->pod.unitControllerType.enumValue != UnitControllerType::RANGED_ELITE)
			{
				projectileUnitMold = td;
			}
		}
		StartCoroutine(OnAppear());
	}

	void LeftFrame::OnBossAppear()
	{
		OnSummon();
	}

	void LeftFrame::OnBossDie()
	{
		if (!summonCorountine.expired())
		{
			DeleteCoroutine(summonCorountine);
		}

		if (!unitFrame.expired() && unitFrame.lock()->IsAlive())
		{
			unitFrame.lock()->SetCurrentHp(0);
		}

		for (auto each : summonUnit)
		{
			if (!each.expired() && each.lock()->IsAlive())
			{
				each.lock()->SetCurrentHp(0);
			}
		}
	}

	void LeftFrame::Update()
	{

	}

	void LeftFrame::SetFrameData(application::editor::UnitData* frameData)
	{
		this->frameData = frameData;
	}

	void LeftFrame::ChangeUnit()
	{
		mesh->SetSelfActive(false);
		auto idle = wanderResources::GetAnimation("SKM_Frame1", UnitAnimType::Idle);
		idle->SetLoop(false);
		unitFrame = UnitPool::SingleInstance().Borrow(frameData);
	}

	void LeftFrame::SummonUnit()
	{
		if (!summonCorountine.expired())
		{
			DeleteCoroutine(summonCorountine);
		}
		summonCorountine = StartCoroutine(SummonMoldUnit());
	}

	bool LeftFrame::IsAlive() const
	{
		return HasChangedUnit() && unitFrame.lock()->IsAlive();
	}

	coroutine::Coroutine LeftFrame::OnAppear()
	{
		auto& gc = GlobalConstant::GetSingletonInstance().pod;

		mesh->SetSelfActive(true);
		coroutine::ForSeconds preAppear{ gc.bossAppearTime };
		mesh->GetTransform()->SetLocalPosition(Vector3d(0, gc.bossAppearHeight, 0));

		auto initVel = wanderUtils::GetInitSpeedOfFreeFall(gc.bossAppearTime, Vector3d(0, gc.bossAppearHeight, 0), Vector3d(0, 1, 0));
		while (preAppear.Tick())
		{
			initVel += Vector3d::down * gc.gravitySpeed * preAppear.Elapsed();
			mesh->GetTransform()->SetLocalPosition(Vector3d(0, (mesh->GetTransform()->GetLocalPosition() + initVel * preAppear.Elapsed()).y, 0));
			auto curPos = mesh->GetTransform()->GetLocalPosition();
			if (curPos.y < 1)
			{
				mesh->GetTransform()->SetLocalPosition(Vector3d(0, 1, 0));
				break;
			}
			co_await std::suspend_always();
		}

		auto animator = GetGameObject()->GetComponent<yunutyEngine::graphics::Animator>();
		auto anim = wanderResources::GetAnimation("SKM_Frame1", UnitAnimType::Birth);
		animator->Play(anim);

		coroutine::ForSeconds forSeconds{ anim->GetDuration() };

		while (forSeconds.Tick())
		{
			co_await std::suspend_always();
		}
		auto idle = wanderResources::GetAnimation("SKM_Frame1", UnitAnimType::Idle);
		idle->SetLoop(true);
		animator->Play(idle);
		co_return;
	}

	coroutine::Coroutine LeftFrame::SummonMoldUnit()
	{
		auto blockFollowingNavigation = unitFrame.lock()->referenceBlockFollowingNavAgent.Acquire();
		auto blockAnimLoop = unitFrame.lock()->referenceBlockAnimLoop.Acquire();
		auto disableNavAgent = unitFrame.lock()->referenceDisableNavAgent.Acquire();
		auto blockStateChange = unitFrame.lock()->referencePause.Acquire();

		unitFrame.lock()->PlayAnimation(UnitAnimType::Skill1, false);
		auto animator = unitFrame.lock()->GetAnimator();
		auto anim = wanderResources::GetAnimation(unitFrame.lock()->GetFBXName(), UnitAnimType::Skill1);
		coroutine::ForSeconds forSeconds{ anim->GetDuration() };

		Vector3d pivotPos = GetGameObject()->GetTransform()->GetWorldPosition() + Vector3d(-BossSummonMobSkill::pod.leftSummonOffset_x, 0, BossSummonMobSkill::pod.leftSummonOffset_z);
		Quaternion summonRot = GetGameObject()->GetTransform()->GetWorldRotation();

		int meleeSummonCount = 0;
		int projectileSummonCount = 0;
		while (forSeconds.Tick())
		{
			bool unitSummon = false;
			auto index = math::Random::GetRandomInt(0, 1);
			if (meleeSummonCount + projectileSummonCount == BossSummonMobSkill::pod.leftMeleeCount + BossSummonMobSkill::pod.leftProjectileCount)
			{
				continue;
			}

			while (forSeconds.ElapsedNormalized() < (float)(meleeSummonCount + projectileSummonCount) / (float)(BossSummonMobSkill::pod.leftMeleeCount + BossSummonMobSkill::pod.leftProjectileCount))
			{
				co_await std::suspend_always();
				continue;
			}

			while (!unitSummon)
			{
				if (meleeSummonCount + projectileSummonCount == BossSummonMobSkill::pod.leftMeleeCount + BossSummonMobSkill::pod.leftProjectileCount)
				{
					break;
				}

				switch (index)
				{
					case 0:
					{
						if (meleeSummonCount < BossSummonMobSkill::pod.leftMeleeCount)
						{
							Vector3d finalPos = Vector3d();
							finalPos.x = math::Random::GetRandomFloat((pivotPos + BossSummonMobSkill::pod.leftNoiseRadius * summonRot.Right().Normalized()).x, pivotPos.x + BossSummonMobSkill::pod.leftNoiseRadius);
							finalPos.z = math::Random::GetRandomFloat(pivotPos.z - BossSummonMobSkill::pod.leftNoiseRadius, (pivotPos - BossSummonMobSkill::pod.leftNoiseRadius * summonRot.Right().Normalized()).z);
							auto sUnit = UnitPool::SingleInstance().Borrow(meleeUnitMold, finalPos, summonRot);
							summonUnit.insert(sUnit);
							/// 소환 사운드
							// SFXManager::PlaySoundfile3D("sounds/", finalPos);

							co_await std::suspend_always{};

							sUnit.lock()->OrderMove(finalPos - summonRot.Forward().Normalized() * std::sqrt(BossSummonMobSkill::pod.leftSummonOffset_x * BossSummonMobSkill::pod.leftSummonOffset_x + BossSummonMobSkill::pod.leftSummonOffset_z * BossSummonMobSkill::pod.leftSummonOffset_z));
							meleeSummonCount++;
							unitSummon = true;
						}
						break;
					}
					case 1:
					{
						if (projectileSummonCount < BossSummonMobSkill::pod.leftProjectileCount)
						{
							Vector3d finalPos = Vector3d();
							finalPos.x = math::Random::GetRandomFloat((pivotPos + BossSummonMobSkill::pod.leftNoiseRadius * summonRot.Right().Normalized()).x, pivotPos.x + BossSummonMobSkill::pod.leftNoiseRadius);
							finalPos.z = math::Random::GetRandomFloat(pivotPos.z - BossSummonMobSkill::pod.leftNoiseRadius, (pivotPos - BossSummonMobSkill::pod.leftNoiseRadius * summonRot.Right().Normalized()).z);
							auto sUnit = UnitPool::SingleInstance().Borrow(projectileUnitMold, finalPos, summonRot);
							summonUnit.insert(sUnit);
							/// 소환 사운드
							// SFXManager::PlaySoundfile3D("sounds/", finalPos);

							co_await std::suspend_always{};
							sUnit.lock()->OrderMove(finalPos - summonRot.Forward().Normalized() * std::sqrt(BossSummonMobSkill::pod.leftSummonOffset_x * BossSummonMobSkill::pod.leftSummonOffset_x + BossSummonMobSkill::pod.leftSummonOffset_z * BossSummonMobSkill::pod.leftSummonOffset_z));
							projectileSummonCount++;
							unitSummon = true;
						}
						break;
					}
					default:
						break;
				}
			}
		}
		co_return;
	}
}
