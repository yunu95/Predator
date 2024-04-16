/// 2024. 04. 11 김상준
/// Region 에서 벗어날 때 호출되는 Trigger 입니다.

#pragma once

#include "ITrigger.h"

namespace application
{
	namespace editor
	{
		class Module_ScriptEditor;
		class RegionData;
	}

	class Trigger_LeaveRegion
		: public ITrigger
	{
		friend class editor::Module_ScriptEditor;

	public:
		DEFINE_TRIGGER(LeaveRegion)

		virtual void LinkCallback() override;

		void SetRegion(editor::RegionData* region);

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

	private:
		editor::RegionData* enteringRegion;
		bool isEditing = false;
	};
}
