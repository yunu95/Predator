/// 2024. 05. 30 김상준
/// Chess Object 를 Boss 등 소환할 경우에 사용할 Chess Pool 입니다.

#pragma once
#include "YunutyEngine.h"
#include "DebugMeshes.h"
#include "PermanentObservee.h"


namespace BossSummon
{
	class ChessObject;
	class ChessPawn;
	class ChessRook;
	class ChessBishop;
}

namespace application
{
	namespace editor
	{
		class ITemplateData;
	}
}

namespace BossSummon
{
	struct CustomCompChessObject
	{
		bool operator()(const std::weak_ptr<ChessObject>& lp, const std::weak_ptr<ChessObject>& rp) const
		{
			return lp.lock().get() > rp.lock().get();
		}
	};
}

/// 실제 사용자가 사용하게 될 Pool 입니다.
/// 내부적으로 Chess Pool 을 통해서 Object 를 관리합니다.
namespace BossSummon
{
	class ChessPool
		: public SingletonComponent<ChessPool>, public Component, public PermanentObservee
	{
	public:
		virtual void Start() override;

		virtual void OnContentsStop() override; 

		std::weak_ptr<ChessPawn> BorrowPawn();
		std::weak_ptr<ChessRook> BorrowRook();
		std::weak_ptr<ChessBishop> BorrowBishop();
		void Return(std::weak_ptr<ChessPawn> chess);
		void Return(std::weak_ptr<ChessRook> chess);
		void Return(std::weak_ptr<ChessBishop> chess);

		virtual Component* GetComponent() { return this; }

		application::editor::ITemplateData* GetPawnTemplate();
		application::editor::ITemplateData* GetRookTemplate();
		application::editor::ITemplateData* GetBishopTemplate();

		void OnBossDie();

		static std::set<std::weak_ptr<ChessObject>, CustomCompChessObject>& GetSummonChesses();

	private:
		std::map<std::weak_ptr<ChessObject>, int, CustomCompChessObject> borrowedChess = std::map<std::weak_ptr<ChessObject>, int, CustomCompChessObject>();
	};
}
