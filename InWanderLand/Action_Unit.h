/// 2024. 05. 10 김상준
/// Unit 에 관련된 Action 목록입니다. 

#pragma once

#include "IAction.h"

#include "PlayableComponent.h"

#include "YunuGraphicsInterface.h"

namespace yunutyEngine
{
	class GameObject;
}

namespace application
{
	namespace editor
	{
		class Module_ScriptEditor;
		class UnitData;
	}

	/// 특정 위치로 Unit 의 상태를 이동하는 Action 입니다.
	class Action_UnitMove
		: public IAction
	{
		friend class editor::Module_ScriptEditor;

	public:
		DEFINE_ACTION(UnitMove)

		virtual ~Action_UnitMove();

		virtual CoroutineObject<void> DoAction() override;

		virtual bool IsValid() override;

		void SetTargetUnit(editor::UnitData* unit);
		void SetDestinationUnit(editor::UnitData* unit);

		virtual void ProcessObervationEvent(ObservationTarget* target, ObservationEvent event) override;

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

	private:
		editor::UnitData* targetUnit = nullptr;
		editor::UnitData* destinationUnit = nullptr;
		bool isEditing = false;
	};
}
