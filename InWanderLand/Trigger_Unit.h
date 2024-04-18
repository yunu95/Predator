/// 2024. 04. 17 김상준
/// Unit 에 관련된 Trigger 목록입니다. 

#pragma once

#include "ITrigger.h"

namespace application
{
	namespace editor
	{
		class Module_ScriptEditor;
		class UnitData;
	}

	/// Unit 이 나타났을 때 발현되는 Trigger 입니다.
	class Trigger_UnitAppear
		: public ITrigger
	{
		friend class editor::Module_ScriptEditor;

	public:
		DEFINE_TRIGGER(UnitAppear)

		virtual ~Trigger_UnitAppear();

		virtual void LinkCallback() override;

		void SetUnit(editor::UnitData* unit);

		virtual bool IsValid() override;

		virtual void ProcessObervationEvent(ObservationTarget* target, ObservationEvent event) override;

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

	private:
		editor::UnitData* targetUnit = nullptr;
		bool isEditing = false;
	};

	/// Unit 이 사망하였을 때 발현되는 Trigger 입니다.
	class Trigger_UnitDie
		: public ITrigger
	{
		friend class editor::Module_ScriptEditor;

	public:
		DEFINE_TRIGGER(UnitDie)

		virtual ~Trigger_UnitDie();

		virtual void LinkCallback() override;

		void SetUnit(editor::UnitData* unit);

		virtual bool IsValid() override;

		virtual void ProcessObervationEvent(ObservationTarget* target, ObservationEvent event) override;

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

	private:
		editor::UnitData* targetUnit = nullptr;
		bool isEditing = false;
	};
}

