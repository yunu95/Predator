#include "ChessPool.h"

#include "ChessPawn.h"
#include "ChessRook.h"
#include "ChessBishop.h"
#include "GlobalConstant.h"
#include "TemplateDataManager.h"
#include "Interactable_TemplateData.h"
#include "BossSummonChessSkill.h"

namespace BossSummon
{
	class ChessPawnPool :
		public GameObjectPool<ChessPawn>, public SingletonComponent<ChessPawnPool>, public Component
	{
	public:
		virtual void ObjectInitializer(std::weak_ptr<ChessPawn> comp) override
		{
			comp.lock()->Init(templateData);
		}

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

		application::editor::Interactable_TemplateData* templateData = nullptr;
	};

	class ChessRookPool :
		public GameObjectPool<ChessRook>, public SingletonComponent<ChessRookPool>, public Component
	{
	public:
		virtual void ObjectInitializer(std::weak_ptr<ChessRook> comp) override
		{
			comp.lock()->Init(templateData);
		}

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

		application::editor::Interactable_TemplateData* templateData = nullptr;
	};

	class ChessBishopPool :
		public GameObjectPool<ChessBishop>, public SingletonComponent<ChessBishopPool>, public Component
	{
	public:
		virtual void ObjectInitializer(std::weak_ptr<ChessBishop> comp) override
		{
			comp.lock()->Init(templateData);
		}

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

		application::editor::Interactable_TemplateData* templateData = nullptr;
	};

	void ChessPool::Start()
	{
		ChessPawnPool::Instance().MatchTemplateData();
		ChessRookPool::Instance().MatchTemplateData();
		ChessBishopPool::Instance().MatchTemplateData();
	}

	void ChessPool::OnContentsStop()
	{
		for (auto [ptr, index] : borrowedChess)
		{
			switch (index)
			{
				case 0:
				{
					Return(std::static_pointer_cast<ChessPawn>(ptr.lock()));
					break;
				}
				case 1:
				{
					Return(std::static_pointer_cast<ChessRook>(ptr.lock()));
					break;
				}
				case 2:
				{
					Return(std::static_pointer_cast<ChessBishop>(ptr.lock()));
					break;
				}
				default:
					break;
			}
		}
	}

	std::weak_ptr<ChessPawn> ChessPool::BorrowPawn()
	{
		auto wptr = ChessPawnPool::Instance().Borrow();
		wptr.lock()->myWeakPtr = wptr;
		borrowedChess.insert({ wptr, 0 });
		return wptr;
	}

	std::weak_ptr<ChessRook> ChessPool::BorrowRook()
	{
		auto wptr = ChessRookPool::Instance().Borrow();
		wptr.lock()->myWeakPtr = wptr;
		borrowedChess.insert({ wptr, 1 });
		return wptr;
	}

	std::weak_ptr<ChessBishop> ChessPool::BorrowBishop()
	{
		auto wptr = ChessBishopPool::Instance().Borrow();
		wptr.lock()->myWeakPtr = wptr;
		borrowedChess.insert({ wptr, 2 });
		return wptr;
	}

	void ChessPool::Return(std::weak_ptr<ChessPawn> chess)
	{
		if (chess.expired())
		{
			return;
		}
		chess.lock()->OnReturn();
		borrowedChess.erase(chess);
		ChessPawnPool::Instance().Return(chess);
	}

	void ChessPool::Return(std::weak_ptr<ChessRook> chess)
	{
		if (chess.expired())
		{
			return;
		}
		chess.lock()->OnReturn();
		borrowedChess.erase(chess);
		ChessRookPool::Instance().Return(chess);
	}

	void ChessPool::Return(std::weak_ptr<ChessBishop> chess)
	{
		if (chess.expired())
		{
			return;
		}
		chess.lock()->OnReturn();
		borrowedChess.erase(chess);
		ChessBishopPool::Instance().Return(chess);
	}

	application::editor::ITemplateData* ChessPool::GetPawnTemplate()
	{
		return ChessPawnPool::Instance().templateData;;
	}

	application::editor::ITemplateData* ChessPool::GetRookTemplate()
	{
		return ChessRookPool::Instance().templateData;;
	}

	application::editor::ITemplateData* ChessPool::GetBishopTemplate()
	{
		return ChessBishopPool::Instance().templateData;
	}

	void ChessPool::OnBossDie()
	{
		for (auto [each, idx] : borrowedChess)
		{
			each.lock()->OnBossDie();
		}
	}
}
