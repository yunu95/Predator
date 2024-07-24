/// 2024. 07. 24 김상준
/// Interactable 에 관련된 Action 목록입니다. 

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
		class InteractableData;
	}

	/// Interactable 의 Active 를 true 로 바꾸는 Action 입니다.
	class Action_InteractableShow
		: public IAction
	{
		friend class editor::Module_ScriptEditor;

	public:
		DEFINE_ACTION(InteractableShow)

			virtual ~Action_InteractableShow();

		virtual CoroutineObject<void> DoAction() override;

		virtual bool IsValid() override;

		void SetTargetInteractable(editor::InteractableData* particle);

		virtual void ProcessObervationEvent(ObservationTarget* target, ObservationEvent event) override;

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

		virtual void ProgressInitialize() override;
		virtual void CurrentProgressSave() override;
		virtual void Recovery() override;

	private:
		editor::InteractableData* targetInteractable = nullptr;
		bool isEditing = false;
		bool savedActive = true;
	};

	/// Interactable 의 Active 를 false 로 바꾸는 Action 입니다.
	class Action_InteractableHide
		: public IAction
	{
		friend class editor::Module_ScriptEditor;

	public:
		DEFINE_ACTION(InteractableHide)

			virtual ~Action_InteractableHide();

		virtual CoroutineObject<void> DoAction() override;

		virtual bool IsValid() override;

		void SetTargetInteractable(editor::InteractableData* particle);

		virtual void ProcessObervationEvent(ObservationTarget* target, ObservationEvent event) override;

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

		virtual void ProgressInitialize() override;
		virtual void CurrentProgressSave() override;
		virtual void Recovery() override;

	private:
		editor::InteractableData* targetInteractable = nullptr;
		bool isEditing = false;
		bool savedActive = false;
	};
}
