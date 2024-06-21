/// 2024. 06. 21 김상준
/// Ornament 에 관련된 Condition 목록입니다. 

#pragma once

#include "ICondition.h"

namespace application
{
	namespace editor
	{
		class Module_ScriptEditor;
		class OrnamentData;
	}

	class Condition_OrnamentShow
		: public ICondition
	{
		friend class editor::Module_ScriptEditor;

	public:
		DEFINE_CONDITION(OrnamentShow)

		virtual ~Condition_OrnamentShow();

		virtual bool IsConditionMet() override;

		virtual bool IsValid() override;

		void SetTargetOrnament(editor::OrnamentData* ornament);

		virtual void ProcessObervationEvent(ObservationTarget* target, ObservationEvent event) override;

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

	private:
		editor::OrnamentData* targetOrnament = nullptr;
		bool isEditing = false;
	};

	class Condition_OrnamentHide
		: public ICondition
	{
		friend class editor::Module_ScriptEditor;

	public:
		DEFINE_CONDITION(OrnamentHide)

			virtual ~Condition_OrnamentHide();

		virtual bool IsConditionMet() override;

		virtual bool IsValid() override;

		void SetTargetOrnament(editor::OrnamentData* ornament);

		virtual void ProcessObervationEvent(ObservationTarget* target, ObservationEvent event) override;

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

	private:
		editor::OrnamentData* targetOrnament = nullptr;
		bool isEditing = false;
	};
}
