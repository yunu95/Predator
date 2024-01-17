/// 2023. 10. 18 김상준
/// 에디터에서 특정한 명령에 대해 일괄 처리할 수 있는 구조를 탐색하던 중
/// 명령을 호출하는 호출자와 이를 수행하는 객체를 분리하여 사용하는 구조인
/// 명령 패턴(Command Pattern)을 발견하여 이를 사용하기로하여 만들어낸 클래스
/// 
/// Command 클래스는 '인터페이스'로, 해당 클래스를 상속받아 구체화한
/// 하위 Command 클래스에서 명령을 처리하도록 함

#pragma once

#ifdef EDITOR
#include <memory>

namespace application
{
	namespace editor
	{
		class Command
		{
		public:
			virtual void Execute() = 0;	// 해당 커맨드를 실행함
		};
		
		/// Undo / Redo 가능한 Command 는 선별하여 처리할 수 있도록 지원하기 위해
		/// 따로 한 단계 추상화를 더함
		class UndoableCommand 
			: public Command
		{
		public:
			virtual void Execute() = 0;
			virtual void Undo() = 0;	// 해당 커맨드 실행의 이전 상태로 되돌림
		};

		/// 특별한 인자 요청 없이 단순히 처리할 수 있는 명령에 대해서 처리하기 위한
		/// 단순한 유형의 커맨드 템플릿 작성
		template <typename Receiver>
		class SimpleCommands
			: public Command
		{
		public:
			using Action = void (Receiver::*)();

			SimpleCommands(Receiver* receiver, Action action)
				: receiver(receiver), action(action)
			{

			}

			virtual void Execute()
			{
				(receiver->*action)();
			}

		private:
			Action action;		// Execute 시에 실행될 단순한 함수
			Receiver* receiver;	// 커맨드의 대상이 될 수신자
		};
	}
}
#endif
