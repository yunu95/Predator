#include "InWanderLand.h"
#include "BossSummonChessSkill.h"
#include "Interactable_ChessPool.h"
#include "Interactable_ChessPawn.h"
#include "Interactable_ChessRook.h"
#include "Interactable_ChessBishop.h"
#include "GlobalConstant.h"

POD_BossSummonChessSkill BossSummonChessSkill::pod = POD_BossSummonChessSkill();

coroutine::Coroutine BossSummonChessSkill::operator()()
{
	owner.lock()->PlayAnimation(UnitAnimType::Skill4, true);
	auto animator = owner.lock()->GetAnimator();
	auto anim = wanderResources::GetAnimation(owner.lock()->name, UnitAnimType::Skill2);
	coroutine::ForSeconds forSeconds{ anim->GetDuration() };
	Vector3d farUnitPos = Vector3d();
	Vector3d ownerPos = owner.lock()->GetTransform()->GetWorldPosition();
	/// Player 유닛만 받아서 가장 먼 거리 계산
	float distance = 0;
	//for (auto pUnit : PlayerUnitList)
	//{
	//	float tempVal = (pUnit->GetTransform()->GetWorldPosition() - ownerPos).MagnitudeSqr();
	//	if (distance <= tempVal)
	//	{
	//		distance = tempVal;
	//		farUnitPos = pUnit->GetTransform()->GetWorldPosition();
	//	}
	//}

	farUnitPos = ownerPos;
	owner.lock()->StartCoroutine(SummonChess(GetPlaceableIndex(farUnitPos)));
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

coroutine::Coroutine BossSummonChessSkill::SummonChess(Vector2i index)
{
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
	else if (index.x > pod.verticalSpaces - 1 - pod.rectUnitRadius)
	{
		index.y = pod.verticalSpaces - 1 - pod.rectUnitRadius;
	}

	auto& gc = GlobalConstant::GetSingletonInstance().pod;
	float unitLength = gc.chessBlockUnitLength + gc.chessBlockUnitOffset;

	std::unordered_set<std::weak_ptr<Interactable_ChessPawn>> borrowedPawns = std::unordered_set<std::weak_ptr<Interactable_ChessPawn>>();
	std::unordered_set<std::weak_ptr<Interactable_ChessRook>> borrowedRooks = std::unordered_set<std::weak_ptr<Interactable_ChessRook>>();
	std::unordered_set<std::weak_ptr<Interactable_ChessBishop>> borrowedBishops = std::unordered_set<std::weak_ptr<Interactable_ChessBishop>>();
	std::unordered_set<unsigned int> chessIndex = std::unordered_set<unsigned int>();
	
	for (int i = 0; i < pod.summonCount; i++)
	{
		std::weak_ptr<IInteractableComponent> container;
		auto summonIdx = math::Random::GetRandomInt(0, 2);
		switch (summonIdx)
		{
			case 0:
			{
				container = Interactable_ChessPool::Instance().BorrowPawn();
				borrowedPawns.insert(std::static_pointer_cast<Interactable_ChessPawn>(container.lock()));
				break;
			}
			case 1:
			{
				container = Interactable_ChessPool::Instance().BorrowRook();
				borrowedRooks.insert(std::static_pointer_cast<Interactable_ChessRook>(container.lock()));
				break;
			}
			case 2:
			{
				container = Interactable_ChessPool::Instance().BorrowBishop();
				borrowedBishops.insert(std::static_pointer_cast<Interactable_ChessBishop>(container.lock()));
				break;
			}
			default:
				break;
		}

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

		float finalX = pod.pivotPos.x + (index.x - pod.rectUnitRadius + arrIdx % sideLength) * unitLength;
		float finalZ = pod.pivotPos.z + (index.y - pod.rectUnitRadius + arrIdx / sideLength) * unitLength;

		container.lock()->GetTransform()->SetWorldPosition(Vector3d(finalX, pod.offset_Y, finalZ));
	}

	coroutine::ForSeconds forSeconds{ pod.summonTime };
	auto velocity = wanderUtils::GetInitSpeedOfFreeFall(pod.summonTime, Vector3d(0, pod.offset_Y, 0), Vector3d::zero);
	while (forSeconds.Tick())
	{
		velocity += Vector3d::down * gc.gravitySpeed * forSeconds.Elapsed();
		for (auto each : borrowedPawns)
		{
			each.lock()->GetTransform()->SetWorldPosition(each.lock()->GetTransform()->GetWorldPosition() + velocity * forSeconds.Elapsed());
			auto tempPos = each.lock()->GetTransform()->GetWorldPosition();
			if (tempPos.y < 0)
			{
				tempPos.y = 0;
				each.lock()->GetTransform()->SetWorldPosition(tempPos);
			}
		}
		for (auto each : borrowedRooks)
		{
			each.lock()->GetTransform()->SetWorldPosition(each.lock()->GetTransform()->GetWorldPosition() + velocity * forSeconds.Elapsed());
			auto tempPos = each.lock()->GetTransform()->GetWorldPosition();
			if (tempPos.y < 0)
			{
				tempPos.y = 0;
				each.lock()->GetTransform()->SetWorldPosition(tempPos);
			}
		}
		for (auto each : borrowedBishops)
		{
			each.lock()->GetTransform()->SetWorldPosition(each.lock()->GetTransform()->GetWorldPosition() + velocity * forSeconds.Elapsed());
			auto tempPos = each.lock()->GetTransform()->GetWorldPosition();
			if (tempPos.y < 0)
			{
				tempPos.y = 0;
				each.lock()->GetTransform()->SetWorldPosition(tempPos);
			}
		}

		co_await std::suspend_always{};
	}

	auto playDust = [=](GameObject* target)->void
	{
		for (auto each : target->GetChildren())
		{
			if (each->getName() == "DustParticleObj")
			{
				auto pr = each->GetComponent<graphics::ParticleRenderer>();
				pr->Play();
				return;
			}
		}

		auto particleObj = target->AddGameObject();
		particleObj->setName("DustParticleObj");
		auto pScale = target->GetTransform()->GetWorldScale();
		assert(pScale.x != 0 && pScale.y != 0 && pScale.z != 0);
		particleObj->GetTransform()->SetLocalScale(Vector3d(1 / pScale.x, 1 / pScale.y, 1 / pScale.z));
		auto pComp = particleObj->AddComponent<graphics::ParticleRenderer>();
		for (auto each : application::particle::ParticleTool_Manager::GetSingletonInstance().GetParticleList())
		{
			if (each.lock()->name == "DustParticle")
			{
				auto data = each.lock();
				pComp->SetParticleShape((yunutyEngine::graphics::ParticleShape)data->shape);
				pComp->SetParticleMode((yunutyEngine::graphics::ParticleMode)data->particleMode);
				pComp->SetLoop(data->isLoop);
				pComp->SetDuration(data->duration);
				pComp->SetLifeTime(data->lifeTime);
				pComp->SetSpeed(data->speed);
				pComp->SetStartScale(data->startScale);
				pComp->SetEndScale(data->endScale);
				pComp->SetMaxParticle(data->maxParticle);
				pComp->SetPlayAwake(data->playAwake);
				pComp->SetRadius(data->radius);
				pComp->SetAngle(data->angle);

				pComp->SetRateOverTime(data->rateOverTime);

				pComp->SetBurstsCount(data->burstsCount);
				pComp->SetInterval(data->interval);

				static const yunuGI::IResourceManager* resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();

				std::wstring texturePath;
				texturePath.assign(data->texturePath.begin(), data->texturePath.end());
				auto texturePtr = resourceManager->GetTexture(texturePath);
				if (texturePtr)
				{
					pComp->SetTexture(texturePtr);
				}
				break;
			}
		}
	};

	/// 낙하할 때 사운드 관련 작업 필요
	for (auto each : borrowedPawns)
	{
		playDust(each.lock()->GetGameObject());
	}
	for (auto each : borrowedRooks)
	{
		playDust(each.lock()->GetGameObject());
	}
	for (auto each : borrowedBishops)
	{
		playDust(each.lock()->GetGameObject());
	}

	float maxDelayTime = 0;
	float maxParticleTime = 0;
	if (!borrowedPawns.empty())
	{
		maxDelayTime = borrowedPawns.begin()->lock()->delayTime > maxDelayTime ? borrowedPawns.begin()->lock()->delayTime : maxDelayTime;
		maxParticleTime = borrowedPawns.begin()->lock()->particleEffectTime > maxParticleTime ? borrowedPawns.begin()->lock()->particleEffectTime : maxParticleTime;
	}
	if (!borrowedRooks.empty())
	{
		maxDelayTime = borrowedRooks.begin()->lock()->delayTime > maxDelayTime ? borrowedRooks.begin()->lock()->delayTime : maxDelayTime;
		maxParticleTime = borrowedRooks.begin()->lock()->particleEffectTime > maxParticleTime ? borrowedRooks.begin()->lock()->particleEffectTime : maxParticleTime;
	}
	if (!borrowedBishops.empty())
	{
		maxDelayTime = borrowedBishops.begin()->lock()->delayTime > maxDelayTime ? borrowedBishops.begin()->lock()->delayTime : maxDelayTime;
		maxParticleTime = borrowedBishops.begin()->lock()->particleEffectTime > maxParticleTime ? borrowedBishops.begin()->lock()->particleEffectTime : maxParticleTime;
	}

	coroutine::ForSeconds waitBomb{ pod.chessSummonedExplosionDelay + maxDelayTime + maxParticleTime};
	while (waitBomb.Tick())
	{
		co_await std::suspend_always{};
	}
	
	for (auto each : borrowedPawns)
	{
		Interactable_ChessPool::Instance().Return(each);
	}
	for (auto each : borrowedRooks)
	{
		Interactable_ChessPool::Instance().Return(each);
	}
	for (auto each : borrowedBishops)
	{
		Interactable_ChessPool::Instance().Return(each);
	}
	co_return;
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
			horizontalMap.insert({ pod.pivotPos.x + i * unitLength, i});
		}
	}

	if (pod.verticalSpaces != beforeSizeH)
	{
		verticalMap.clear();
		for (int i = 0; i < pod.verticalSpaces - 1; i++)
		{
			verticalMap.insert({ pod.pivotPos.z + i * unitLength, i });
		}
	}

	int finalIndexX = horizontalMap.lower_bound(pos.x)->second;
	int finalIndexZ = verticalMap.lower_bound(pos.z)->second;

	return Vector2i(finalIndexX, finalIndexZ);
}
