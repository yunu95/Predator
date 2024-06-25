/// 2024. 06. 25 김상준
/// Stage 와 관련된 Trigger 목록입니다.

#pragma once

#include "ITrigger.h"

namespace application
{
	class Trigger_StageStart
		: public ITrigger
	{
	public:
		DEFINE_TRIGGER(StageStart)

		virtual void LinkCallback() override;

		void SetStageIndex(int index);

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

	private:
		int index = 1;
	};
}

