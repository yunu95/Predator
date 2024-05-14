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

	/// 특정 위치로 Unit 의 Position 을 이동하는 Action 입니다.
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

	/// Unit 을 제자리에서 회전하는 Action 입니다.
	/// isRelative 가 true 일 경우, 현재 Rotation 에서 상대적으로 회전합니다.
	class Action_UnitRotate
		: public IAction
	{
		friend class editor::Module_ScriptEditor;

	public:
		DEFINE_ACTION(UnitRotate)

		virtual ~Action_UnitRotate();

		virtual CoroutineObject<void> DoAction() override;

		virtual bool IsValid() override;

		void SetTargetUnit(editor::UnitData* unit);
		void SetRelative(bool isRelative);
		void SetRotation(float angle);
		void SetLerpTime(float lerpTime);

		virtual void ProcessObervationEvent(ObservationTarget* target, ObservationEvent event) override;

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

	private:
		editor::UnitData* targetUnit = nullptr;
		bool isRelative = false;
		float angle = 0;
		float lerpTime = 0;
		bool isEditing = false;
	};

	/// Unit 을 제자리에서 Rescale 하는 Action 입니다.
	class Action_UnitRescale
		: public IAction
	{
		friend class editor::Module_ScriptEditor;

	public:
		DEFINE_ACTION(UnitRescale)

		virtual ~Action_UnitRescale();

		virtual CoroutineObject<void> DoAction() override;

		virtual bool IsValid() override;

		void SetTargetUnit(editor::UnitData* unit);
		void SetFinalScale(const yunuGI::Vector3& scale);
		void SetLerpTime(float lerpTime);

		virtual void ProcessObervationEvent(ObservationTarget* target, ObservationEvent event) override;

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

	private:
		editor::UnitData* targetUnit = nullptr;
		yunuGI::Vector3 scale = yunuGI::Vector3(1, 1, 1);
		float lerpTime = 0;
		bool isEditing = false;
	};

	/// 특정 위치로 Unit 의 Position 을 이동한 뒤,
	/// Rotate 와 Rescale 을 수행하는 Action 입니다.
	class Action_UnitMoveWithRotateAndRescale
		: public IAction
	{
		friend class editor::Module_ScriptEditor;

	public:
		DEFINE_ACTION(UnitMoveWithRotateAndRescale)

		virtual ~Action_UnitMoveWithRotateAndRescale();

		virtual CoroutineObject<void> DoAction() override;

		virtual bool IsValid() override;

		void SetTargetUnit(editor::UnitData* unit);
		void SetDestinationUnit(editor::UnitData* unit);
		void SetLerpTime(float lerpTime);

		virtual void ProcessObervationEvent(ObservationTarget* target, ObservationEvent event) override;

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

	private:
		editor::UnitData* targetUnit = nullptr;
		editor::UnitData* destinationUnit = nullptr;
		float lerpTime = 0;
		bool isEditing = false;
	};
}
