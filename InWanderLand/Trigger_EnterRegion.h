#pragma once
/// 2024. 04. 09 김상준
/// Region 에 들어갈 때 호출되는 Trigger 입니다.

#pragma once

#include "ITrigger.h"

#include "Singleton.h"

namespace application
{
	namespace editor
	{
		class RegionData;
	}

	class Trigger_EnterRegion
		: public ITrigger
	{
	public:
		DEFINE_TRIGGER(EnterRegion)

		virtual void LinkCallback() override;

		void SetRegion(editor::RegionData* region);

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

	private:
		editor::RegionData* enteringRegion;
	};
}
