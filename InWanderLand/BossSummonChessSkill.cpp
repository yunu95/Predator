#include "InWanderLand.h"
#include "BossSummonChessSkill.h"
#include "ChessPool.h"
#include "ChessPawn.h"
#include "ChessRook.h"
#include "ChessBishop.h"
#include "GlobalConstant.h"
#include "YunutyWaitForSeconds.h"
#include "VFXAnimator.h"

POD_BossSummonChessSkill BossSummonChessSkill::pod = POD_BossSummonChessSkill();

BossSummonChessSkill::BossSummonChessSkill()
{
	BossSummon::ChessPool::Instance().Start();
}

coroutine::Coroutine BossSummonChessSkill::operator()()
{
	auto blockFollowingNavigation = owner.lock()->referenceBlockFollowingNavAgent.Acquire();
	auto blockAnimLoop = owner.lock()->referenceBlockAnimLoop.Acquire();
	auto disableNavAgent = owner.lock()->referenceDisableNavAgent.Acquire();
	auto rotRef = owner.lock()->referenceBlockRotation.Acquire();

	owner.lock()->PlayAnimation(UnitAnimType::Skill4, Animation::PlayFlag_::Blending | Animation::PlayFlag_::Repeat);
	effectCoroutine = owner.lock()->StartCoroutine(SpawningFieldEffect(std::dynamic_pointer_cast<BossSummonChessSkill>(selfWeakPtr.lock())));
	effectCoroutine.lock()->PushDestroyCallBack([this]()
		{
			FBXPool::Instance().Return(stepEffect);
		});
	auto animator = owner.lock()->GetAnimator();
	auto anim = wanderResources::GetAnimation(owner.lock()->GetFBXName(), UnitAnimType::Skill4);
	wanderUtils::UnitCoroutine::ForSecondsFromUnit forSeconds{ owner, anim->GetDuration() };

	Vector3d farUnitPos = Vector3d();
	Vector3d ownerPos = owner.lock()->GetTransform()->GetWorldPosition();
	/// Player 유닛만 받아서 가장 먼 거리 계산
	float distance = 0;
	for (auto pUnit : PlayerController::Instance().GetPlayers())
	{
		if (pUnit.expired())
		{
			continue;
		}
		float tempVal = (pUnit.lock()->GetTransform()->GetWorldPosition() - ownerPos).MagnitudeSqr();
		if (pUnit.lock()->IsAlive() && distance <= tempVal)
		{
			distance = tempVal;
			farUnitPos = pUnit.lock()->GetTransform()->GetWorldPosition();
		}
	}

	owner.lock()->StartCoroutine(SummonChess(std::static_pointer_cast<BossSummonChessSkill>(selfWeakPtr.lock()), GetPlaceableIndex(farUnitPos)));
	while (forSeconds.Tick())
	{
		co_await std::suspend_always{};
	}

	disableNavAgent.reset();
	blockFollowingNavigation.reset();
	owner.lock()->Relocate(owner.lock()->GetTransform()->GetWorldPosition());
	co_return;
}

void BossSummonChessSkill::OnInterruption()
{
	if (!effectCoroutine.expired())
	{
		owner.lock()->DeleteCoroutine(effectCoroutine);
	}
}

void BossSummonChessSkill::OnPause()
{
	if (!stepEffectAnimator.expired())
	{
		stepEffectAnimator.lock()->Pause();
	}
}

void BossSummonChessSkill::OnResume()
{
	if (!stepEffectAnimator.expired())
	{
		stepEffectAnimator.lock()->Resume();
	}
}

void BossSummonChessSkill::OnBossDie()
{
	BossSummon::ChessPool::Instance().OnBossDie();
}

coroutine::Coroutine BossSummonChessSkill::SpawningFieldEffect(std::weak_ptr<BossSummonChessSkill> skill)
{
	Vector3d startPos = owner.lock()->GetTransform()->GetWorldPosition();
	Vector3d deltaPos = targetPos - owner.lock()->GetTransform()->GetWorldPosition();
	Vector3d direction = deltaPos.Normalized();
	Vector3d endPos = startPos + deltaPos;
	Vector3d currentPos = startPos;

	stepEffect = FBXPool::Instance().Borrow("VFX_HeartQueen_Skill3_1");
	stepEffect.lock()->GetGameObject()->GetTransform()->SetWorldPosition(startPos);
	stepEffect.lock()->GetGameObject()->GetTransform()->SetWorldRotation(owner.lock()->GetTransform()->GetWorldRotation());
	stepEffect.lock()->GetGameObject()->GetTransform()->SetWorldScale(owner.lock()->GetTransform()->GetWorldScale());

	stepEffectAnimator = stepEffect.lock()->AcquireVFXAnimator();
	stepEffectAnimator.lock()->SetAutoActiveFalse();
	stepEffectAnimator.lock()->Init();
	stepEffectAnimator.lock()->Play();

	auto anim = wanderResources::GetAnimation(owner.lock()->GetFBXName(), UnitAnimType::Skill3);

	wanderUtils::UnitCoroutine::ForSecondsFromUnit forSeconds{ owner, anim->GetDuration() };
	while (forSeconds.Tick())
	{
		co_await std::suspend_always();
	}
	
	co_return;
}

coroutine::Coroutine BossSummonChessSkill::SummonChess(std::weak_ptr<BossSummonChessSkill> skill, Vector2i index)
{
	auto sptr = skill.lock();

	if (index.x < pod.rectUnitRadius)
	{
		index.x = pod.rectUnitRadius;
	}
	else if (index.x > pod.horizontalSpaces - 1 - pod.rectUnitRadius)
	{
		index.x = pod.horizontalSpaces - 1 - pod.rectUnitRadius;
	}

	if (index.y < pod.rectUnitRadius)
	{
		index.y = pod.rectUnitRadius;
	}
	else if (index.y > pod.verticalSpaces - 1 - pod.rectUnitRadius)
	{
		index.y = pod.verticalSpaces - 1 - pod.rectUnitRadius;
	}

	auto& gc = GlobalConstant::GetSingletonInstance().pod;
	float unitLength = gc.chessBlockUnitLength + gc.chessBlockUnitOffset;

	std::unordered_set<unsigned int> chessIndex = std::unordered_set<unsigned int>();
	std::vector<std::weak_ptr<BossSummon::ChessObject>> chessList = std::vector<std::weak_ptr<BossSummon::ChessObject>>();
	std::vector<bool> ready = std::vector<bool>(pod.summonCount, false);
	for (int i = 0; i < pod.summonCount; i++)
	{
		int arrIdx = -1;
		int sideLength = pod.rectUnitRadius * 2 + 1;
		while (true)
		{
			arrIdx = math::Random::GetRandomInt(0, sideLength * sideLength - 1);
			if (!chessIndex.contains(arrIdx))
			{
				chessIndex.insert(arrIdx);
				break;
			}
		}

		float finalX = pod.pivotPos_x + (index.x - pod.rectUnitRadius + arrIdx % sideLength) * unitLength;
		float finalZ = pod.pivotPos_z + (index.y - pod.rectUnitRadius + arrIdx / sideLength) * unitLength;

		auto summonIdx = math::Random::GetRandomInt(0, 2);
		switch (summonIdx)
		{
			case 0:
			{
				auto pawn = BossSummon::ChessPool::Instance().BorrowPawn();
				pawn.lock()->OnSummon();
				pawn.lock()->GetTransform()->SetWorldPosition(Vector3d(finalX, pod.offset_Y, finalZ));
				borrowedPawns.insert(pawn);
				chessList.push_back(std::static_pointer_cast<BossSummon::ChessObject>(pawn.lock()));
				break;
			}
			case 1:
			{
				auto rook = BossSummon::ChessPool::Instance().BorrowRook();
				rook.lock()->OnSummon();
				rook.lock()->GetTransform()->SetWorldPosition(Vector3d(finalX, pod.offset_Y, finalZ));
				borrowedRooks.insert(rook);
				chessList.push_back(std::static_pointer_cast<BossSummon::ChessObject>(rook.lock()));
				break;
			}
			case 2:
			{
				auto bishop = BossSummon::ChessPool::Instance().BorrowBishop();
				bishop.lock()->OnSummon();
				bishop.lock()->GetTransform()->SetWorldPosition(Vector3d(finalX, pod.offset_Y, finalZ));
				borrowedBishops.insert(bishop);
				chessList.push_back(std::static_pointer_cast<BossSummon::ChessObject>(bishop.lock()));
				break;
			}
			default:
				break;
		}
	}

	wanderUtils::UnitCoroutine::ForSecondsFromUnit preSeconds{ owner, pod.summonPreDelay };

	while (preSeconds.Tick())
	{
		co_await std::suspend_always{};
	}

	wanderUtils::UnitCoroutine::ForSecondsFromUnit forSeconds{ owner, pod.summonTime };

	Vector3d indivVelocity = Vector3d();
	int summoned = 1;
	if (!pod.intervalSummon)
	{
		summoned = pod.summonCount;
		indivVelocity = wanderUtils::GetInitSpeedOfFreeFall(pod.summonTime, Vector3d(0, pod.offset_Y, 0), Vector3d::zero);
	}
	else
	{
		assert(pod.summonCount != 0);
		indivVelocity = wanderUtils::GetInitSpeedOfFreeFall(pod.summonTime / pod.summonCount, Vector3d(0, pod.offset_Y, 0), Vector3d::zero);
	}
	std::vector<Vector3d> velocityList = std::vector<Vector3d>(pod.summonCount, indivVelocity);

	while (forSeconds.Tick())
	{
		if (pod.intervalSummon && forSeconds.ElapsedNormalized() < 1 && forSeconds.ElapsedNormalized() >= 1 / (float)pod.summonCount * summoned)
		{
			summoned++;
		}

		for (int i = 0; i < summoned; i++)
		{
			velocityList[i] += Vector3d::down * gc.gravitySpeed * forSeconds.GetDeltaTime();
			chessList[i].lock()->GetSummonComponent()->GetTransform()->SetWorldPosition(chessList[i].lock()->GetSummonComponent()->GetTransform()->GetWorldPosition() + velocityList[i] * forSeconds.GetDeltaTime());
			auto tempPos = chessList[i].lock()->GetSummonComponent()->GetTransform()->GetWorldPosition();
			if (tempPos.y <= 0)
			{
				tempPos.y = 0;
				chessList[i].lock()->GetSummonComponent()->GetTransform()->SetWorldPosition(tempPos);
				if (!ready[i])
				{
					chessList[i].lock()->SetReady();
					SFXManager::PlaySoundfile3D("sounds/Heart Queen/Heart Queen chess summon.wav", tempPos);
					ready[i] = true;
				}
			}

		}
		co_await std::suspend_always{};
	}

	for (auto each : chessList)
	{
		each.lock()->StartTimer();
	}
	co_await std::suspend_always{};
}

Vector2i BossSummonChessSkill::GetPlaceableIndex(Vector3d pos)
{
	auto& gc = GlobalConstant::GetSingletonInstance().pod;
	float unitLength = gc.chessBlockUnitLength + gc.chessBlockUnitOffset;

	static std::map<float, int> horizontalMap;
	static std::map<float, int> verticalMap;

	static int beforeSizeW = -1;
	static int beforeSizeH = -1;

	if (pod.horizontalSpaces != beforeSizeW)
	{
		horizontalMap.clear();
		for (int i = 0; i < pod.horizontalSpaces - 1; i++)
		{
			horizontalMap.insert({ pod.pivotPos_x - unitLength / 2 + i * unitLength, i });
		}

		horizontalMap.insert({ pod.pivotPos_x - unitLength / 2 + pod.horizontalSpaces * unitLength,  pod.horizontalSpaces - 1 });
		horizontalMap.insert({ pod.pivotPos_x - unitLength / 2 + (pod.horizontalSpaces + 1) * unitLength,  pod.horizontalSpaces - 1 });
	}

	if (pod.verticalSpaces != beforeSizeH)
	{
		verticalMap.clear();
		for (int j = 0; j < pod.verticalSpaces; j++)
		{
			verticalMap.insert({ pod.pivotPos_z - unitLength / 2 + j * unitLength, j });
		}

		verticalMap.insert({ pod.pivotPos_z - unitLength / 2 + pod.verticalSpaces * unitLength,  pod.verticalSpaces - 1 });
		verticalMap.insert({ pod.pivotPos_z - unitLength / 2 + (pod.verticalSpaces + 1) * unitLength,  pod.verticalSpaces - 1 });
	}

	int finalIndexX = horizontalMap.lower_bound(pos.x)->second;
	int finalIndexZ = verticalMap.lower_bound(pos.z)->second;

	return Vector2i(finalIndexX, finalIndexZ);
}
