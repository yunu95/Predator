#include "InWanderLand.h"
#include "BossSummonChessSkill.h"
#include "ChessPool.h"
#include "ChessPawn.h"
#include "ChessRook.h"
#include "ChessBishop.h"
#include "GlobalConstant.h"
#include "YunutyWaitForSeconds.h"

POD_BossSummonChessSkill BossSummonChessSkill::pod = POD_BossSummonChessSkill();

BossSummonChessSkill::BossSummonChessSkill()
{
	BossSummon::ChessPool::Instance().Start();
}

coroutine::Coroutine BossSummonChessSkill::operator()()
{
	owner.lock()->PlayAnimation(UnitAnimType::Skill4, true);
	auto animator = owner.lock()->GetAnimator();
	auto anim = wanderResources::GetAnimation(owner.lock()->GetFBXName(), UnitAnimType::Skill4);
	coroutine::ForSeconds forSeconds{ anim->GetDuration() };
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
	OnInterruption();
	co_return;
}

void BossSummonChessSkill::OnInterruption()
{

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

	coroutine::ForSeconds preSeconds{ pod.summonPreDelay };

	while (preSeconds.Tick())
	{
		co_await std::suspend_always{};
	}

	coroutine::ForSeconds forSeconds{ pod.summonTime };
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

		auto itr = chessList.begin();
		for (int i = 0; i < summoned; i++)
		{
			velocityList[i] += Vector3d::down * gc.gravitySpeed * forSeconds.Elapsed();
			chessList[i].lock()->GetSummonComponent()->GetTransform()->SetWorldPosition(chessList[i].lock()->GetSummonComponent()->GetTransform()->GetWorldPosition() + velocityList[i] * forSeconds.Elapsed());
			auto tempPos = chessList[i].lock()->GetSummonComponent()->GetTransform()->GetWorldPosition();
			if (tempPos.y <= 0)
			{
				tempPos.y = 0;
				chessList[i].lock()->GetSummonComponent()->GetTransform()->SetWorldPosition(tempPos);
				/// 낙하할 때 사운드 관련 작업 필요
				chessList[i].lock()->SetReady();
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
			horizontalMap.insert({ pod.pivotPos_x + unitLength / 2 + i * unitLength, i });
		}
	}

	if (pod.verticalSpaces != beforeSizeH)
	{
		verticalMap.clear();
		for (int i = 0; i < pod.verticalSpaces - 1; i++)
		{
			verticalMap.insert({ pod.pivotPos_z + unitLength / 2 + i * unitLength, i });
		}
	}

	int finalIndexX = horizontalMap.lower_bound(pos.x)->second;
	int finalIndexZ = verticalMap.lower_bound(pos.z)->second;

	return Vector2i(finalIndexX, finalIndexZ);
}
