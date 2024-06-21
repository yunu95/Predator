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
#include "VFXAnimator.h"

namespace BossSummon
{
	std::weak_ptr<ManagedFBX> LeftFrame::summonEffect = std::weak_ptr<ManagedFBX>();
	std::weak_ptr<ManagedFBX> LeftFrame::summoningEffect = std::weak_ptr<ManagedFBX>();
	application::editor::Unit_TemplateData* LeftFrame::meleeUnitMold = nullptr;
	application::editor::Unit_TemplateData* LeftFrame::projectileUnitMold = nullptr;

	LeftFrame::~LeftFrame()
	{
		summonEffect = std::weak_ptr<ManagedFBX>();
		summoningEffect = std::weak_ptr<ManagedFBX>();
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
		unitFrame.lock()->SetDefaultAnimation(UnitAnimType::Idle);
		unitFrame.lock()->OnStateEngageCallback()[UnitBehaviourTree::Keywords::Death].AddVolatileCallback(
			[this]() 
			{
				if (!summonCorountine.expired())
				{
					DeleteCoroutine(summonCorountine);
				}

				if (!summonEffect.expired())
				{
					FBXPool::Instance().Return(summonEffect);
				}
				if (!summoningEffect.expired())
				{
					FBXPool::Instance().Return(summoningEffect);
				}
			});
	}

	void LeftFrame::SummonUnit()
	{
		if (!IsAlive())
		{
			return;
		}

		if (!summonCorountine.expired())
		{
			DeleteCoroutine(summonCorountine);
		}
		summonCorountine = StartCoroutine(SummonMoldUnit());
		summonCorountine.lock()->PushDestroyCallBack([]()
			{
				if (!summonEffect.expired())
				{
					FBXPool::Instance().Return(summonEffect);
				}
				if (!summoningEffect.expired())
				{
					FBXPool::Instance().Return(summoningEffect);
				}
			});
	}

	void LeftFrame::OnPause()
	{
		isPause = true;

		if (!HasChangedUnit())
		{
			GetGameObject()->GetComponent<yunutyEngine::graphics::Animator>()->Pause();
		}

		if (!summonEffectAnimator.expired())
		{
			summonEffectAnimator.lock()->Pause();
		}

		if (!summoningEffectAnimator.expired())
		{
			summoningEffectAnimator.lock()->Pause();
		}
	}

	void LeftFrame::OnResume()
	{
		isPause = false;

		if (!HasChangedUnit())
		{
			GetGameObject()->GetComponent<yunutyEngine::graphics::Animator>()->Resume();
		}

		if (!summonEffectAnimator.expired())
		{
			summonEffectAnimator.lock()->Resume();
		}

		if (!summoningEffectAnimator.expired())
		{
			summoningEffectAnimator.lock()->Resume();
		}
	}

	bool LeftFrame::IsAlive() const
	{
		return HasChangedUnit() && unitFrame.lock()->GetGameObject()->GetActive() && unitFrame.lock()->IsAlive();
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
			initVel += Vector3d::down * gc.gravitySpeed * Time::GetDeltaTime();
			mesh->GetTransform()->SetLocalPosition(Vector3d(0, (mesh->GetTransform()->GetLocalPosition() + initVel * Time::GetDeltaTime()).y, 0));
			auto curPos = mesh->GetTransform()->GetLocalPosition();
			if (curPos.y < 1)
			{
				mesh->GetTransform()->SetLocalPosition(Vector3d(0, 1, 0));
				break;
			}
			co_await std::suspend_always();
		}

		mesh->GetTransform()->SetLocalPosition(Vector3d(0, 0, 0));
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

		co_await std::suspend_always();

		unitFrame.lock()->PlayAnimation(UnitAnimType::Skill1, Animation::PlayFlag_::None);
		auto animator = unitFrame.lock()->GetAnimator();
		auto anim = wanderResources::GetAnimation(unitFrame.lock()->GetFBXName(), UnitAnimType::Skill1);
		wanderUtils::UnitCoroutine::ForSecondsFromUnit forSeconds{ unitFrame, anim->GetDuration() };

		Vector3d pivotPos = GetGameObject()->GetTransform()->GetWorldPosition() + Vector3d(-BossSummonMobSkill::pod.leftSummonOffset_x, 0, BossSummonMobSkill::pod.leftSummonOffset_z);
		Quaternion summonRot = GetGameObject()->GetTransform()->GetWorldRotation();

		/// 1. VFX 실행을 한다(Set Auto 어쩌고로 내가 Active 관리)
		summonEffect = FBXPool::Instance().Borrow("VFX_Frame_Summon");
		summonEffect.lock()->GetGameObject()->GetTransform()->SetWorldPosition(GetTransform()->GetWorldPosition() + GetTransform()->GetWorldRotation().Forward() * -1.5);
		
		auto rot = GetTransform()->GetWorldRotation();
		auto euler = rot.Euler();
		euler.y += 180;

		summonEffect.lock()->GetGameObject()->GetTransform()->SetWorldRotation(Quaternion{ euler });

		summonEffectAnimator = summonEffect.lock()->AcquireVFXAnimator();
		summonEffectAnimator.lock()->SetAutoActiveFalse();
		summonEffectAnimator.lock()->Init();

		summoningEffect = FBXPool::Instance().Borrow("VFX_Frame_Summoning");
		summoningEffect.lock()->GetGameObject()->GetTransform()->SetWorldPosition(GetTransform()->GetWorldPosition() + GetTransform()->GetWorldRotation().Forward() * -1.5);
		summoningEffect.lock()->GetGameObject()->GetTransform()->SetWorldRotation(Quaternion{ euler });
		summoningEffectAnimator = summoningEffect.lock()->AcquireVFXAnimator();
		summoningEffectAnimator.lock()->SetLoop(true);
		summoningEffectAnimator.lock()->SetAutoActiveFalse();
		summoningEffectAnimator.lock()->Init();
		summoningEffect.lock()->GetGameObject()->SetSelfActive(false);

		summonEffectAnimator.lock()->Play();
		/// 2. 특정 시간 만큼 기다린다. (VFX Summon Animation 끝날 때까지 co_await suspend always / IsDone)
		while (!summonEffectAnimator.lock()->IsDone())
		{
			co_await std::suspend_always{};
		}
		/// 3. VFX 바꿔서 실행한다(VFX Summoning, 기존 거는 Active false 하고(?) 반납)
		summonEffect.lock()->GetGameObject()->SetSelfActive(false);
		summoningEffect.lock()->GetGameObject()->SetSelfActive(true);
		blockAnimLoop.reset();
		//unitFrame.lock()->PlayAnimation(UnitAnimType::Idle, Animation::PlayFlag_::Blending | Animation::PlayFlag_::Repeat);
		summoningEffectAnimator.lock()->Play();

		co_await std::suspend_always{};

		int meleeSummonCount = 0;
		int projectileSummonCount = 0;
		while (forSeconds.Tick())
		{
			if (forSeconds.ElapsedNormalized() < (float)(meleeSummonCount + projectileSummonCount) / (float)(BossSummonMobSkill::pod.leftMeleeCount + BossSummonMobSkill::pod.leftProjectileCount))
			{
				co_await std::suspend_always();
				continue;
			}

			if (meleeSummonCount + projectileSummonCount == BossSummonMobSkill::pod.leftMeleeCount + BossSummonMobSkill::pod.leftProjectileCount)
			{
				continue;
			}

			bool unitSummon = false;
			while (!unitSummon)
			{
				auto index = math::Random::GetRandomInt(0, 1);
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

							int findCount = 0;
							while (findCount < PlayerCharacterType::Num)
							{
								int targetIndex = math::Random::GetRandomInt(1, PlayerCharacterType::Num);
								auto targetUnit = PlayerController::Instance().GetPlayers().at(targetIndex - 1);
								if (!targetUnit.expired() && targetUnit.lock()->IsAlive())
								{
									sUnit.lock()->OrderAttackMove(targetUnit.lock()->GetTransform()->GetWorldPosition());
									break;
								}
								findCount++;
							}

							//sUnit.lock()->OrderMove(finalPos - summonRot.Forward().Normalized() * std::sqrt(BossSummonMobSkill::pod.leftSummonOffset_x * BossSummonMobSkill::pod.leftSummonOffset_x + BossSummonMobSkill::pod.leftSummonOffset_z * BossSummonMobSkill::pod.leftSummonOffset_z));
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

							int findCount = 0;
							while (findCount < PlayerCharacterType::Num)
							{
								int targetIndex = math::Random::GetRandomInt(1, PlayerCharacterType::Num);
								auto targetUnit = PlayerController::Instance().GetPlayers().at(targetIndex - 1);
								if (!targetUnit.expired() && targetUnit.lock()->IsAlive())
								{
									sUnit.lock()->OrderAttackMove(targetUnit.lock()->GetTransform()->GetWorldPosition());
									break;
								}
								findCount++;
							}

							//sUnit.lock()->OrderMove(finalPos - summonRot.Forward().Normalized() * std::sqrt(BossSummonMobSkill::pod.leftSummonOffset_x * BossSummonMobSkill::pod.leftSummonOffset_x + BossSummonMobSkill::pod.leftSummonOffset_z * BossSummonMobSkill::pod.leftSummonOffset_z));
							projectileSummonCount++;
							unitSummon = true;
						}
						break;
					}
					default:
						break;
				}
			}
			co_await std::suspend_always{};
		}

		FBXPool::Instance().Return(summonEffect);
		FBXPool::Instance().Return(summoningEffect);

		co_return;
	}
}
