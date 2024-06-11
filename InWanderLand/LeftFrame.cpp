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

namespace BossSummon
{
	std::vector<application::editor::UnitData*> LeftFrame::mold = std::vector<application::editor::UnitData*>();

	void LeftFrame::RegisterUnitData(application::editor::UnitData* unitData)
	{
		mold.push_back(unitData);
	}

	LeftFrame::~LeftFrame()
	{
		OnBossDie();
		mold.clear();
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
		StartCoroutine(OnAppear());
	}

	void LeftFrame::OnBossAppear()
	{
		OnSummon();
	}

	void LeftFrame::OnBossDie()
	{
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
		unitFrame.lock()->StartCoroutine(SummonMoldUnit());
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
		unitFrame.lock()->PlayAnimation(UnitAnimType::Skill1, true);
		auto animator = unitFrame.lock()->GetAnimator();
		auto anim = wanderResources::GetAnimation(unitFrame.lock()->GetFBXName(), UnitAnimType::Skill1);
		coroutine::ForSeconds forSeconds{ anim->GetDuration() };

		Vector3d pivotPos = GetGameObject()->GetTransform()->GetWorldPosition() + Vector3d(-BossSummonMobSkill::pod.leftSummonOffset_x, 0, BossSummonMobSkill::pod.leftSummonOffset_z);
		Quaternion summonRot = GetGameObject()->GetTransform()->GetWorldRotation();

		int summonCount = 0;
		while (forSeconds.Tick())
		{
			if (summonCount < mold.size() && forSeconds.ElapsedNormalized() >= 1 / (float)mold.size() * summonCount)
			{
				Vector3d finalPos = Vector3d();
				finalPos.x = math::Random::GetRandomFloat((pivotPos + BossSummonMobSkill::pod.leftNoiseRadius * summonRot.Right().Normalized()).x, pivotPos.x + BossSummonMobSkill::pod.leftNoiseRadius);
				finalPos.z = math::Random::GetRandomFloat(pivotPos.z - BossSummonMobSkill::pod.leftNoiseRadius, (pivotPos - BossSummonMobSkill::pod.leftNoiseRadius * summonRot.Right().Normalized()).z);
				auto sUnit = UnitPool::SingleInstance().Borrow(mold[summonCount++]);
				sUnit.lock()->GetTransform()->SetWorldPosition(finalPos);
				sUnit.lock()->GetTransform()->SetWorldRotation(summonRot);
				summonUnit.insert(sUnit);

				/// 소환 사운드
				// SFXManager::PlaySoundfile3D("sounds/", finalPos);

				/// 소환 후 이동 명령
				sUnit.lock()->OrderMove(finalPos + summonRot.Forward().Normalized() * std::sqrt(BossSummonMobSkill::pod.leftSummonOffset_x * BossSummonMobSkill::pod.leftSummonOffset_x + BossSummonMobSkill::pod.leftSummonOffset_z * BossSummonMobSkill::pod.leftSummonOffset_z));
				co_await std::suspend_always{};
			}
		}

		unitFrame.lock()->PlayAnimation(UnitAnimType::Idle, true);
		co_return;
	}
}
