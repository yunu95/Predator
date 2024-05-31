/// 2024. 05. 29 김상준
/// Interactable 에 관련된 Trigger 목록입니다. 

#pragma once

#include "ITrigger.h"

namespace application
{
	namespace editor
	{
		class Module_ScriptEditor;
		class InteractableData;
	}

	/// Interactable 이 Enter 상태가 되었을 때 발현되는 Trigger 입니다.
	class Trigger_InteractableTriggerEnter
		: public ITrigger
	{
		friend class editor::Module_ScriptEditor;

	public:
		DEFINE_TRIGGER(InteractableTriggerEnter)

		virtual ~Trigger_InteractableTriggerEnter();

		virtual void LinkCallback() override;

		void SetInteractable(editor::InteractableData* interactable);

		virtual bool IsValid() override;

		virtual void ProcessObervationEvent(ObservationTarget* target, ObservationEvent event) override;

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

	private:
		editor::InteractableData* targetInteractable = nullptr;
		bool isEditing = false;
	};

	/// Interactable 이 Exit 상태가 되었을 때 발현되는 Trigger 입니다.
	class Trigger_InteractableTriggerExit
		: public ITrigger
	{
		friend class editor::Module_ScriptEditor;

	public:
		DEFINE_TRIGGER(InteractableTriggerExit)

		virtual ~Trigger_InteractableTriggerExit();

		virtual void LinkCallback() override;

		void SetInteractable(editor::InteractableData* unit);

		virtual bool IsValid() override;

		virtual void ProcessObervationEvent(ObservationTarget* target, ObservationEvent event) override;

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

	private:
		editor::InteractableData* targetInteractable = nullptr;
		bool isEditing = false;
	};
}

