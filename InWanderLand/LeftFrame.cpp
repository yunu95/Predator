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
		mold.clear();
		BossSummonMobSkill::SetLeftFrame(nullptr);
	}

	void LeftFrame::Init(application::editor::ITemplateData* templateData)
	{
		for (auto each : GetGameObject()->GetChildren())
		{
			auto renderer = each->GetComponent<graphics::StaticMeshRenderer>();
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
		/// Pool 에서 Summon 하는 게 아니라, 해당 함수로 Boss 등장 시 Animation 재생합니다.
		mesh->SetSelfActive(true);
		//auto animator = GetGameObject()->GetComponent<yunutyEngine::graphics::Animator>();
		//auto anim = wanderResources::GetAnimation("SKM_FRAME1", UnitAnimType::Birth);
		//animator->Play(anim);
	}

	void LeftFrame::OnBossDie()
	{
		if (!unitFrame.expired())
		{
			/// unitFrame 을 죽여서 사망 애니메이션이 나오도록 합니다.
			unitFrame.lock();
		}

		for (auto each : summonUnit)
		{
			if (!each.expired() && each.lock()->IsAlive())
			{
				/// 죽이기
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
			if (forSeconds.ElapsedNormalized() > 1 / (float)mold.size() * summonCount)
			{
				co_await std::suspend_always{};
				continue;
			}

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

		unitFrame.lock()->PlayAnimation(UnitAnimType::Idle, true);
		co_return;
	}
}
