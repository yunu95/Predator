/// 2024. 06. 25 김상준
/// Particle 에 관련된 Action 목록입니다. 

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
		class ParticleData;
	}

	/// 특정 위치로 Particle 의 Position, Rotate, Rescale 을 동시에 수행하는 Action 입니다.
	/// Particle 의 경우, Move 로직이 특별하지 않기에 독립적인 Move / Rotate / Rescale 없이 해당 Action 만 사용합니다.
	class Action_ParticleMoveWithRotateAndRescale
		: public IAction
	{
		friend class editor::Module_ScriptEditor;

	public:
		DEFINE_ACTION(ParticleMoveWithRotateAndRescale)

			virtual ~Action_ParticleMoveWithRotateAndRescale();

		virtual CoroutineObject<void> DoAction() override;

		virtual bool IsValid() override;

		void SetTargetParticle(editor::ParticleData* particle);
		void SetDestinationParticle(editor::ParticleData* particle);
		void SetLerpTime(float lerpTime);

		virtual void ProcessObervationEvent(ObservationTarget* target, ObservationEvent event) override;

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

		virtual void PostRecovery() override;

	private:
		editor::ParticleData* targetParticle = nullptr;
		editor::ParticleData* destinationParticle = nullptr;
		float lerpTime = 0;
		bool isEditing = false;
	};

	/// Particle 의 Active 를 true 로 바꾸는 Action 입니다.
	class Action_ParticleShow
		: public IAction
	{
		friend class editor::Module_ScriptEditor;

	public:
		DEFINE_ACTION(ParticleShow)

			virtual ~Action_ParticleShow();

		virtual CoroutineObject<void> DoAction() override;

		virtual bool IsValid() override;

		void SetTargetParticle(editor::ParticleData* particle);

		virtual void ProcessObervationEvent(ObservationTarget* target, ObservationEvent event) override;

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

		virtual void ProgressInitialize() override;
		virtual void CurrentProgressSave() override;
		virtual void Recovery() override;

	private:
		editor::ParticleData* targetParticle = nullptr;
		bool isEditing = false;
		bool savedActive = true;
	};

	/// Particle 의 Active 를 false 로 바꾸는 Action 입니다.
	class Action_ParticleHide
		: public IAction
	{
		friend class editor::Module_ScriptEditor;

	public:
		DEFINE_ACTION(ParticleHide)

			virtual ~Action_ParticleHide();

		virtual CoroutineObject<void> DoAction() override;

		virtual bool IsValid() override;

		void SetTargetParticle(editor::ParticleData* particle);

		virtual void ProcessObervationEvent(ObservationTarget* target, ObservationEvent event) override;

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

		virtual void ProgressInitialize() override;
		virtual void CurrentProgressSave() override;
		virtual void Recovery() override;

	private:
		editor::ParticleData* targetParticle = nullptr;
		bool isEditing = false;
		bool savedActive = false;
	};
}
