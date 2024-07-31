#include "RightFrame.h"

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
#include "VFXAnimator.h"

namespace BossSummon
{
	std::weak_ptr<ManagedFBX> RightFrame::summonEffect = std::weak_ptr<ManagedFBX>();
	std::weak_ptr<ManagedFBX> RightFrame::summoningEffect = std::weak_ptr<ManagedFBX>();
	application::editor::Unit_TemplateData* RightFrame::meleeUnitMold = nullptr;
	application::editor::Unit_TemplateData* RightFrame::weakMeleeUnitMold = nullptr;
	application::editor::Unit_TemplateData* RightFrame::eliteMeleeUnitMold = nullptr;
	
	application::editor::Unit_TemplateData* RightFrame::projectileUnitMold = nullptr;
	application::editor::Unit_TemplateData* RightFrame::weakProjectileUnitMold = nullptr;
	application::editor::Unit_TemplateData* RightFrame::weakApproachProjectileUnitMold = nullptr;
	application::editor::Unit_TemplateData* RightFrame::kitingProjectileUnitMold = nullptr;
	application::editor::Unit_TemplateData* RightFrame::siegeProjectileUnitMold = nullptr;
	application::editor::Unit_TemplateData* RightFrame::eliteProjectileUnitMold = nullptr;
	
	RightFrame::~RightFrame()
	{
		summonEffect = std::weak_ptr<ManagedFBX>();
		summoningEffect = std::weak_ptr<ManagedFBX>();
		meleeUnitMold = nullptr;
		weakMeleeUnitMold = nullptr;
		eliteMeleeUnitMold = nullptr;

		projectileUnitMold = nullptr;
		weakProjectileUnitMold = nullptr;
		weakApproachProjectileUnitMold = nullptr;
		kitingProjectileUnitMold = nullptr;
		siegeProjectileUnitMold = nullptr;
		eliteProjectileUnitMold = nullptr;
		BossSummonMobSkill::SetRightFrame(nullptr);

		disNav.reset();
	}

	void RightFrame::Init(application::editor::ITemplateData* templateData)
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

	void RightFrame::OnSummon()
	{
		for (auto each : application::editor::TemplateDataManager::GetSingletonInstance().GetDataList(application::editor::DataType::UnitData))
		{
			auto td = static_cast<application::editor::Unit_TemplateData*>(each);

			if (td->pod.skinnedFBXName == "SKM_Monster1")
			{
				if (td->pod.unit_scale < 1.0f)
				{
					weakMeleeUnitMold = td;
				}
				else if (td->pod.unitControllerType.enumValue == UnitControllerType::ANGRY_MOB)
				{
					meleeUnitMold = td;
				}
				else if (td->pod.unitControllerType.enumValue == UnitControllerType::MELEE_ELITE)
				{
					eliteMeleeUnitMold = td;
				}
				else
				{
					meleeUnitMold = td;
				}
			}
			else if (td->pod.skinnedFBXName == "SKM_Monster2")
			{
				if (td->pod.unit_scale < 1.0f && td->pod.unitControllerType.enumValue == UnitControllerType::RANGED_APPROACHING)
				{
					weakApproachProjectileUnitMold = td;
				}
				else if (td->pod.unit_scale < 1.0f && td->pod.unitControllerType.enumValue != UnitControllerType::RANGED_APPROACHING)
				{
					weakProjectileUnitMold = td;
				}
				else if (td->pod.unitControllerType.enumValue == UnitControllerType::ANGRY_MOB)
				{
					projectileUnitMold = td;
				}
				else if (td->pod.unitControllerType.enumValue == UnitControllerType::RANGED_KITING)
				{
					kitingProjectileUnitMold = td;
				}
				else if (td->pod.unitControllerType.enumValue == UnitControllerType::HOLDER)
				{
					siegeProjectileUnitMold = td;
				}
				else if (td->pod.unitControllerType.enumValue == UnitControllerType::RANGED_ELITE)
				{
					eliteProjectileUnitMold = td;
				}
				else
				{
					projectileUnitMold = td;
				}
			}
		}
		StartCoroutine(OnAppear());
	}

	void RightFrame::OnBossAppear()
	{
		OnSummon();
	}

	void RightFrame::OnBossDie()
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

	void RightFrame::Update()
	{

	}

	void RightFrame::SetFrameData(application::editor::UnitData* frameData)
	{
		this->frameData = frameData;
	}

	void RightFrame::ChangeUnit()
	{
		mesh->SetSelfActive(false);
		auto idle = wanderResources::GetAnimation("SKM_Frame2", UnitAnimType::Idle);
		idle->SetLoop(false);
		unitFrame = UnitPool::SingleInstance().Borrow(frameData);
		unitFrame.lock()->belongingWave = nullptr;
		disNav = unitFrame.lock()->referenceDisableNavAgent.Acquire();
		Vector3d framePos = Vector3d(frameData->pod.position.x, frameData->pod.position.y, frameData->pod.position.z);
		unitFrame.lock()->GetTransform()->SetWorldPosition(framePos);
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
					summonEffect.lock()->GetTransform()->SetWorldRotation(Quaternion::Identity());
					FBXPool::Instance().Return(summonEffect);
				}
				if (!summoningEffect.expired())
				{
					summoningEffect.lock()->GetTransform()->SetWorldRotation(Quaternion::Identity());
					FBXPool::Instance().Return(summoningEffect);
				}
			});
	}

	void RightFrame::SummonUnit()
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
		summonCorountine.lock()->PushDestroyCallBack([this]()
			{
				if (!summonEffect.expired())
				{
					summonEffect.lock()->GetTransform()->SetWorldRotation(Quaternion::Identity());
					FBXPool::Instance().Return(summonEffect);
				}
				if (!summoningEffect.expired())
				{
					summoningEffect.lock()->GetTransform()->SetWorldRotation(Quaternion::Identity());
					FBXPool::Instance().Return(summoningEffect);
				}
			});
	}

	void RightFrame::OnPause()
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

	void RightFrame::OnResume()
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

	void RightFrame::Recovery()
	{
		ClearCoroutines();
		if (mesh)
		{
			mesh->SetSelfActive(false);
		}
		unitFrame.reset();
	}

	bool RightFrame::IsAlive() const
	{
		return HasChangedUnit() && unitFrame.lock()->IsAlive();
	}

	coroutine::Coroutine RightFrame::OnAppear()
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
		auto anim = wanderResources::GetAnimation("SKM_Frame2", UnitAnimType::Birth);
		animator->Play(anim);

		coroutine::ForSeconds forSeconds{ anim->GetDuration() };

		while (forSeconds.Tick())
		{
			co_await std::suspend_always();
		}
		auto idle = wanderResources::GetAnimation("SKM_Frame2", UnitAnimType::Idle);
		idle->SetLoop(true);
		animator->Play(idle);
		co_return;
	}

	coroutine::Coroutine RightFrame::SummonMoldUnit()
	{
		auto blockFollowingNavigation = unitFrame.lock()->referenceBlockFollowingNavAgent.Acquire();
		auto blockAnimLoop = unitFrame.lock()->referenceBlockAnimLoop.Acquire();
		auto disableNavAgent = unitFrame.lock()->referenceDisableNavAgent.Acquire();
		auto blockStateChange = unitFrame.lock()->referencePause.Acquire();
		auto blockRotate = unitFrame.lock()->referenceBlockRotation();

		co_await std::suspend_always();

		unitFrame.lock()->PlayAnimation(UnitAnimType::Skill1);
		auto animator = unitFrame.lock()->GetAnimator();

		Vector3d pivotPos = GetGameObject()->GetTransform()->GetWorldPosition() + Vector3d(BossSummonMobSkill::pod.rightSummonOffset_x, 0, BossSummonMobSkill::pod.rightSummonOffset_z);
		Quaternion summonRot = GetGameObject()->GetTransform()->GetWorldRotation();
		Vector3d effectPosition = GetTransform()->GetWorldPosition() + GetTransform()->GetWorldRotation().Up() * 0.3f;

		summonEffect = FBXPool::Instance().Borrow("VFX_Frame_Summon");
		auto summonEffectRot = GetGameObject()->GetTransform()->GetWorldRotation();
		auto summonEffectEuler = summonEffectRot.Euler();
		summonEffectEuler.x += 7.5f;
		summonEffectEuler.y += 180;

		summonEffect.lock()->GetGameObject()->GetTransform()->SetWorldPosition(effectPosition);
		summonEffect.lock()->GetGameObject()->GetTransform()->SetWorldRotation(Quaternion{ summonEffectEuler });

		summonEffectAnimator = summonEffect.lock()->AcquireVFXAnimator();
		summonEffectAnimator.lock()->SetAutoActiveFalse();
		summonEffectAnimator.lock()->Init();
		summonEffect.lock()->GetGameObject()->SetSelfActive(false);

		summoningEffect = FBXPool::Instance().Borrow("VFX_Frame_Summoning");
		auto summoningEffectRot = GetGameObject()->GetTransform()->GetWorldRotation();
		auto summoningEffectEuler = summoningEffectRot.Euler();
		summoningEffectEuler.x += 7.5f;
		summoningEffectEuler.y += 180;

		summoningEffect.lock()->GetGameObject()->GetTransform()->SetWorldPosition(effectPosition);
		summoningEffect.lock()->GetGameObject()->GetTransform()->SetWorldRotation(Quaternion(summoningEffectEuler));		

		summoningEffectAnimator = summoningEffect.lock()->AcquireVFXAnimator();
		summoningEffectAnimator.lock()->SetLoop(true);
		summoningEffectAnimator.lock()->SetAutoActiveFalse();
		summoningEffectAnimator.lock()->Init();
		summoningEffect.lock()->GetGameObject()->SetSelfActive(false);

		auto anim = wanderResources::GetAnimation(unitFrame.lock()->GetFBXName(), UnitAnimType::Skill1);
		wanderUtils::UnitCoroutine::ForSecondsFromUnit forSeconds{ unitFrame, anim->GetDuration() };
		while (forSeconds.Tick())
		{
			co_await std::suspend_always{};
		}

		//summonEffect.lock()->GetGameObject()->SetSelfActive(true);
		summonEffectAnimator.lock()->Play();
		summonEffect.lock()->GetGameObject()->SetSelfActive(true);
		/// 2. 특정 시간 만큼 기다린다. (VFX Summon Animation 끝날 때까지 co_await suspend always / IsDone)
		wanderUtils::UnitCoroutine::ForSecondsFromUnit forTimeOffSetSeconds{ unitFrame, summonEffectAnimator.lock()->GetDuration()};
		while (forTimeOffSetSeconds.Tick())
		{
			co_await std::suspend_always();
		}

		//while (!summonEffectAnimator.lock()->IsDone())
		//{
		//	co_await std::suspend_always{};
		//}

		/// 3. VFX 바꿔서 실행한다(VFX Summoning, 기존 거는 Active false 하고(?) 반납)
		summonEffect.lock()->GetGameObject()->SetSelfActive(false);
		summoningEffect.lock()->GetGameObject()->SetSelfActive(true);
		//unitFrame.lock()->PlayAnimation(UnitAnimType::Idle, Animation::PlayFlag_::Blending | Animation::PlayFlag_::Repeat);
		summoningEffectAnimator.lock()->Play();

		co_await std::suspend_always{};

		int meleeSummonCount = 0;
		int projectileSummonCount = 0;

		int totalMeleeCount = BossSummonMobSkill::pod.rightMeleeCount + BossSummonMobSkill::pod.rightWeakMeleeEnemyCount + BossSummonMobSkill::pod.rightEliteMeleeEnemyCount;
		int totalRangedCount = BossSummonMobSkill::pod.rightProjectileCount + BossSummonMobSkill::pod.rightWeakRangedCount + BossSummonMobSkill::pod.rightWeakApproachRangedCount + BossSummonMobSkill::pod.rightKitingRangedCount +
			BossSummonMobSkill::pod.rightSiegeRangedCount + BossSummonMobSkill::pod.rightEliteRangedEnemyCount;
		int totalCount = totalMeleeCount + totalRangedCount;

		std::set <std::pair< application::editor::Unit_TemplateData*, int >> meleeCountList = std::set <std::pair< application::editor::Unit_TemplateData*, int >>();
		meleeCountList.insert({ meleeUnitMold, BossSummonMobSkill::pod.rightMeleeCount });
		meleeCountList.insert({ weakMeleeUnitMold, BossSummonMobSkill::pod.rightWeakMeleeEnemyCount });
		meleeCountList.insert({ eliteMeleeUnitMold, BossSummonMobSkill::pod.rightEliteMeleeEnemyCount });

		std::set <std::pair< application::editor::Unit_TemplateData*, int >> rangedCountList = std::set <std::pair< application::editor::Unit_TemplateData*, int >>();
		rangedCountList.insert({ projectileUnitMold, BossSummonMobSkill::pod.rightProjectileCount });
		rangedCountList.insert({ weakProjectileUnitMold, BossSummonMobSkill::pod.rightWeakRangedCount });
		rangedCountList.insert({ weakApproachProjectileUnitMold, BossSummonMobSkill::pod.rightWeakApproachRangedCount });
		rangedCountList.insert({ kitingProjectileUnitMold, BossSummonMobSkill::pod.rightKitingRangedCount });
		rangedCountList.insert({ siegeProjectileUnitMold, BossSummonMobSkill::pod.rightSiegeRangedCount });
		rangedCountList.insert({ eliteProjectileUnitMold, BossSummonMobSkill::pod.rightEliteRangedEnemyCount });

		int currentSummonMeleeUnitCount = 0;
		int currentSummonRangedUnitCount = 0;

		wanderUtils::UnitCoroutine::ForSecondsFromUnit forSummonSeconds{ unitFrame, BossSummonMobSkill::pod.summoningTime };

		while (forSummonSeconds.Tick())
		{
			if (forSummonSeconds.ElapsedNormalized() < (float)(meleeSummonCount + projectileSummonCount) / (float)(totalCount))
			{
				co_await std::suspend_always();
				continue;
			}

			if (meleeSummonCount + projectileSummonCount == totalCount)
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
						if (meleeSummonCount < totalMeleeCount && !meleeCountList.empty())
						{
							if (meleeCountList.begin()->second <= 0)
							{
								meleeCountList.erase(meleeCountList.begin());
								break;
							}

							Vector3d finalPos = Vector3d();
							finalPos.x = math::Random::GetRandomFloat((pivotPos + BossSummonMobSkill::pod.rightNoiseRadius * summonRot.Right().Normalized()).x, pivotPos.x + BossSummonMobSkill::pod.rightNoiseRadius);
							finalPos.z = math::Random::GetRandomFloat(pivotPos.z - BossSummonMobSkill::pod.rightNoiseRadius, (pivotPos - BossSummonMobSkill::pod.rightNoiseRadius * summonRot.Right().Normalized()).z);

							auto sUnit = UnitPool::SingleInstance().Borrow(meleeCountList.begin()->first, finalPos, summonRot);
							sUnit.lock()->belongingWave = nullptr;
							summonUnit.insert(sUnit);
							currentSummonMeleeUnitCount++;
							if (currentSummonMeleeUnitCount >= meleeCountList.begin()->second)
							{
								currentSummonMeleeUnitCount = 0;
								meleeCountList.erase(meleeCountList.begin());
							}

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

							//sUnit.lock()->OrderMove(finalPos - summonRot.Forward().Normalized() * std::sqrt(BossSummonMobSkill::pod.rightSummonOffset_x * BossSummonMobSkill::pod.rightSummonOffset_x + BossSummonMobSkill::pod.rightSummonOffset_z * BossSummonMobSkill::pod.rightSummonOffset_z));
							meleeSummonCount++;
							unitSummon = true;
						}
						break;
					}
					case 1:
					{
						if (projectileSummonCount < totalRangedCount && !rangedCountList.empty())
						{
							if (rangedCountList.begin()->second <= 0)
							{
								rangedCountList.erase(rangedCountList.begin());
								break;
							}
							Vector3d finalPos = Vector3d();
							finalPos.x = math::Random::GetRandomFloat((pivotPos + BossSummonMobSkill::pod.rightNoiseRadius * summonRot.Right().Normalized()).x, pivotPos.x + BossSummonMobSkill::pod.rightNoiseRadius);
							finalPos.z = math::Random::GetRandomFloat(pivotPos.z - BossSummonMobSkill::pod.rightNoiseRadius, (pivotPos - BossSummonMobSkill::pod.rightNoiseRadius * summonRot.Right().Normalized()).z);
							auto sUnit = UnitPool::SingleInstance().Borrow(rangedCountList.begin()->first, finalPos, summonRot);
							sUnit.lock()->belongingWave = nullptr;
							summonUnit.insert(sUnit);
							currentSummonRangedUnitCount++;
							if (currentSummonRangedUnitCount >= rangedCountList.begin()->second)
							{
								currentSummonRangedUnitCount = 0;
								rangedCountList.erase(rangedCountList.begin());
							}
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

							//sUnit.lock()->OrderMove(finalPos - summonRot.Forward().Normalized() * std::sqrt(BossSummonMobSkill::pod.rightSummonOffset_x * BossSummonMobSkill::pod.rightSummonOffset_x + BossSummonMobSkill::pod.rightSummonOffset_z * BossSummonMobSkill::pod.rightSummonOffset_z));
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

		//FBXPool::Instance().Return(summonEffect);
		//FBXPool::Instance().Return(summoningEffect);

		co_return;
	}
}
