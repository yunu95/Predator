#include "Interactable_ChessPool.h"

#include "Interactable_ChessPawn.h"
#include "Interactable_ChessRook.h"
#include "Interactable_ChessBishop.h"
#include "GlobalConstant.h"
#include "TemplateDataManager.h"
#include "Interactable_TemplateData.h"
#include "BossSummonChessSkill.h"

class Interactable_ChessPawnPool :
	public GameObjectPool<Interactable_ChessPawn>, public SingletonComponent<Interactable_ChessPawnPool>, public Component, public PermanentObservee
{
public:
	virtual void ObjectInitializer(std::weak_ptr<Interactable_ChessPawn> comp) override
	{
		comp.lock()->isSummoned = true;

		comp.lock()->chessSummonedExplosionDelay = BossSummonChessSkill::pod.summonTime + BossSummonChessSkill::pod.chessSummonedExplosionDelay;
		comp.lock()->chessBlockUnitLength = application::GlobalConstant::GetSingletonInstance().pod.chessBlockUnitLength;
		comp.lock()->chessBlockUnitOffset = application::GlobalConstant::GetSingletonInstance().pod.chessBlockUnitOffset;
		comp.lock()->vibeMaxOffset = application::GlobalConstant::GetSingletonInstance().pod.vibeMaxOffset;

		assert(templateData && "templateData must already be matched.");
		comp.lock()->damage = templateData->pod.damage;
		comp.lock()->delayTime = templateData->pod.delayTime;
		comp.lock()->particleEffectTime = templateData->pod.particleEffectTime;
	}

	virtual void OnContentsPlay() override
	{
		this->SetActive(true);
		MatchTemplateData();
	}

	virtual void OnContentsStop() override
	{
		this->SetActive(false);
		poolObjects.clear();
		expendableObjects.clear();
	}

	virtual Component* GetComponent() override { return this; }

	void MatchTemplateData()
	{
		/// 해당 FBX templateData 는 하나만 있다고 가정합니다.
		for (auto each : application::editor::TemplateDataManager::GetSingletonInstance().GetDataList(application::editor::DataType::InteractableData))
		{
			if (static_cast<application::editor::Interactable_TemplateData*>(each)->pod.fBXName == "SM_Chess_Pawn")
			{
				templateData = static_cast<application::editor::Interactable_TemplateData*>(each);
				break;
			}
		}
	}

private:
	application::editor::Interactable_TemplateData* templateData = nullptr;
};

class Interactable_ChessRookPool :
	public GameObjectPool<Interactable_ChessRook>, public SingletonComponent<Interactable_ChessRookPool>, public Component, public PermanentObservee
{
public:
	virtual void ObjectInitializer(std::weak_ptr<Interactable_ChessRook> comp) override
	{
		comp.lock()->isSummoned = true;

		comp.lock()->chessSummonedExplosionDelay = BossSummonChessSkill::pod.summonTime + BossSummonChessSkill::pod.chessSummonedExplosionDelay;
		comp.lock()->chessBlockUnitLength = application::GlobalConstant::GetSingletonInstance().pod.chessBlockUnitLength;
		comp.lock()->chessBlockUnitOffset = application::GlobalConstant::GetSingletonInstance().pod.chessBlockUnitOffset;
		comp.lock()->vibeMaxOffset = application::GlobalConstant::GetSingletonInstance().pod.vibeMaxOffset;

		assert(templateData && "templateData must already be matched.");
		comp.lock()->damage = templateData->pod.damage;
		comp.lock()->delayTime = templateData->pod.delayTime;
		comp.lock()->particleEffectTime = templateData->pod.particleEffectTime;
	}

	virtual void OnContentsStop() override
	{
		this->SetActive(false);
		poolObjects.clear();
		expendableObjects.clear();
	}

	virtual Component* GetComponent() override { return this; }

	void MatchTemplateData()
	{
		/// 해당 FBX templateData 는 하나만 있다고 가정합니다.
		for (auto each : application::editor::TemplateDataManager::GetSingletonInstance().GetDataList(application::editor::DataType::InteractableData))
		{
			if (static_cast<application::editor::Interactable_TemplateData*>(each)->pod.fBXName == "SM_Chess_Rook")
			{
				templateData = static_cast<application::editor::Interactable_TemplateData*>(each);
				break;
			}
		}
	}

private:
	application::editor::Interactable_TemplateData* templateData = nullptr;
};

class Interactable_ChessBishopPool :
	public GameObjectPool<Interactable_ChessBishop>, public SingletonComponent<Interactable_ChessBishopPool>, public Component, public PermanentObservee
{
public:
	virtual void ObjectInitializer(std::weak_ptr<Interactable_ChessBishop> comp) override
	{
		comp.lock()->isSummoned = true;

		comp.lock()->chessSummonedExplosionDelay = BossSummonChessSkill::pod.summonTime + BossSummonChessSkill::pod.chessSummonedExplosionDelay;
		comp.lock()->chessBlockUnitLength = application::GlobalConstant::GetSingletonInstance().pod.chessBlockUnitLength;
		comp.lock()->chessBlockUnitOffset = application::GlobalConstant::GetSingletonInstance().pod.chessBlockUnitOffset;
		comp.lock()->vibeMaxOffset = application::GlobalConstant::GetSingletonInstance().pod.vibeMaxOffset;

		assert(templateData && "templateData must already be matched.");
		comp.lock()->damage = templateData->pod.damage;
		comp.lock()->delayTime = templateData->pod.delayTime;
		comp.lock()->particleEffectTime = templateData->pod.particleEffectTime;
	}

	virtual void OnContentsStop() override
	{
		this->SetActive(false);
		poolObjects.clear();
		expendableObjects.clear();
	}

	virtual Component* GetComponent() override { return this; }

	void MatchTemplateData()
	{
		/// 해당 FBX templateData 는 하나만 있다고 가정합니다.
		for (auto each : application::editor::TemplateDataManager::GetSingletonInstance().GetDataList(application::editor::DataType::InteractableData))
		{
			if (static_cast<application::editor::Interactable_TemplateData*>(each)->pod.fBXName == "SM_Chess_Bishop")
			{
				templateData = static_cast<application::editor::Interactable_TemplateData*>(each);
				break;
			}
		}
	}

private:
	application::editor::Interactable_TemplateData* templateData = nullptr;
};

void Interactable_ChessPool::Start()
{
	Interactable_ChessPawnPool::Instance().MatchTemplateData();
	Interactable_ChessRookPool::Instance().MatchTemplateData();
	Interactable_ChessBishopPool::Instance().MatchTemplateData();

	std::vector<std::weak_ptr<Interactable_ChessPawn>> pawnContainer = std::vector<std::weak_ptr<Interactable_ChessPawn>>();
	std::vector<std::weak_ptr<Interactable_ChessRook>> rookContainer = std::vector<std::weak_ptr<Interactable_ChessRook>>();
	std::vector<std::weak_ptr<Interactable_ChessBishop>> bishopContainer = std::vector<std::weak_ptr<Interactable_ChessBishop>>();
	for (int i = 0; i < initContainerSize; i++)
	{
		pawnContainer.push_back(Interactable_ChessPawnPool::Instance().Borrow());
		rookContainer.push_back(Interactable_ChessRookPool::Instance().Borrow());
		bishopContainer.push_back(Interactable_ChessBishopPool::Instance().Borrow());
	}
	for (int i = 0; i < initContainerSize; i++)
	{
		Interactable_ChessPawnPool::Instance().Return(pawnContainer[i]);
		Interactable_ChessRookPool::Instance().Return(rookContainer[i]);
		Interactable_ChessBishopPool::Instance().Return(bishopContainer[i]);
	}
}

std::weak_ptr<Interactable_ChessPawn> Interactable_ChessPool::BorrowPawn()
{
	return Interactable_ChessPawnPool::Instance().Borrow();
}

std::weak_ptr<Interactable_ChessRook> Interactable_ChessPool::BorrowRook()
{
	return Interactable_ChessRookPool::Instance().Borrow();
}

std::weak_ptr<Interactable_ChessBishop> Interactable_ChessPool::BorrowBishop()
{
	return Interactable_ChessBishopPool::Instance().Borrow();
}

void Interactable_ChessPool::Return(std::weak_ptr<Interactable_ChessPawn> chess)
{
	Interactable_ChessPawnPool::Instance().Return(chess);
}

void Interactable_ChessPool::Return(std::weak_ptr<Interactable_ChessRook> chess)
{
	Interactable_ChessRookPool::Instance().Return(chess);
}

void Interactable_ChessPool::Return(std::weak_ptr<Interactable_ChessBishop> chess)
{
	Interactable_ChessBishopPool::Instance().Return(chess);
}
