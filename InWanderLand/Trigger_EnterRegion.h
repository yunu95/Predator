#pragma once
/// 2024. 04. 09 김상준
/// 게임이 시작되었을 때, 발현되는 Trigger 입니다.

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

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

	private:
		editor::RegionData* enteringRegion;
	};
}
