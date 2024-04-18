/// 2024. 04. 09 김상준
/// Region 에 관련된 Trigger 목록입니다. 

#pragma once

#include "ITrigger.h"

namespace application
{
	namespace editor
	{
		class Module_ScriptEditor;
		class RegionData;
	}

	/// Region 에 들어갔을 때 발현되는 Trigger 입니다.
	class Trigger_EnterRegion
		: public ITrigger
	{
		friend class editor::Module_ScriptEditor;

	public:
		DEFINE_TRIGGER(EnterRegion)

		virtual ~Trigger_EnterRegion();

		virtual void LinkCallback() override;

		void SetRegion(editor::RegionData* region);

		virtual bool IsValid() override;

		virtual void ProcessObervationEvent(ObservationTarget* target, ObservationEvent event) override;

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

	private:
		editor::RegionData* targetRegion = nullptr;
		bool isEditing = false;
	};


	/// Region 에서 벗어났을 때 발현되는 Trigger 입니다.
	class Trigger_LeaveRegion
		: public ITrigger
	{
		friend class editor::Module_ScriptEditor;

	public:
		DEFINE_TRIGGER(LeaveRegion)

			virtual ~Trigger_LeaveRegion();

		virtual void LinkCallback() override;

		void SetRegion(editor::RegionData* region);

		virtual bool IsValid() override;

		virtual void ProcessObervationEvent(ObservationTarget* target, ObservationEvent event) override;

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

	private:
		editor::RegionData* targetRegion;
		bool isEditing = false;
	};
}
