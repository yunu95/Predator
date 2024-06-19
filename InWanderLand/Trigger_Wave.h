/// 2024. 06. 18 김상준
/// Wave 에 관련된 Trigger 목록입니다. 

#pragma once

#include "ITrigger.h"

namespace application
{
	namespace editor
	{
		class Module_ScriptEditor;
		class WaveData;
	}

	/// Wave 가 시작할 때 발현되는 Trigger 입니다.
	/// waveIdx 가 0 이면 wave 시작과 동시에,
	/// waveIdx 가 1 이상이면 해당 Idx 의 wave 가 시작할 때 동작합니다.
	class Trigger_StartWave
		: public ITrigger
	{
		friend class editor::Module_ScriptEditor;

	public:
		DEFINE_TRIGGER(StartWave)

		virtual ~Trigger_StartWave();

		virtual void LinkCallback() override;

		void SetWave(editor::WaveData* wave);
		void SetWaveIndex(unsigned int waveIdx);

		virtual bool IsValid() override;

		virtual void ProcessObervationEvent(ObservationTarget* target, ObservationEvent event) override;

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

	private:
		editor::WaveData* targetWave = nullptr;
		bool isEditing = false;
		unsigned int waveIdx = 0;
	};


	/// Wave 가 끝날 때 발현되는 Trigger 입니다.
	/// waveIdx 가 0 이면 wave 유닛들이 모두 사망하였을 때,
	/// waveIdx 가 1 이상이면 해당 Idx 의 wave 유닛들이 모두 사망하였을 때 동작합니다.
	class Trigger_EndWave
		: public ITrigger
	{
		friend class editor::Module_ScriptEditor;

	public:
		DEFINE_TRIGGER(EndWave)

		virtual ~Trigger_EndWave();

		virtual void LinkCallback() override;

		void SetWave(editor::WaveData* region);
		void SetWaveIndex(unsigned int waveIdx);

		virtual bool IsValid() override;

		virtual void ProcessObervationEvent(ObservationTarget* target, ObservationEvent event) override;

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

	private:
		editor::WaveData* targetWave = nullptr;
		bool isEditing = false;
		unsigned int waveIdx = 0;
	};
}
