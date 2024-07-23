/// 2024. 05. 31 김상준
/// Ornament 에 관련된 Action 목록입니다. 

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
		class OrnamentData;
	}

	/// 특정 위치로 Ornament 의 Position, Rotate, Rescale 을 동시에 수행하는 Action 입니다.
	/// Ornament 의 경우, Move 로직이 특별하지 않기에 독립적인 Move / Rotate / Rescale 없이 해당 Action 만 사용합니다.
	class Action_OrnamentMoveWithRotateAndRescale
		: public IAction
	{
		friend class editor::Module_ScriptEditor;

	public:
		DEFINE_ACTION(OrnamentMoveWithRotateAndRescale)

		virtual ~Action_OrnamentMoveWithRotateAndRescale();

		virtual CoroutineObject<void> DoAction() override;

		virtual bool IsValid() override;

		void SetTargetOrnament(editor::OrnamentData* ornament);
		void SetDestinationOrnament(editor::OrnamentData* ornament);
		void SetLerpTime(float lerpTime);

		virtual void ProcessObervationEvent(ObservationTarget* target, ObservationEvent event) override;

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

		virtual void Recovery() override;

	private:
		editor::OrnamentData* targetOrnament = nullptr;
		editor::OrnamentData* destinationOrnament = nullptr;
		float lerpTime = 0;
		bool isEditing = false;
	};

	/// Ornament 의 Active 를 true 로 서서히 바꾸는 Action 입니다.
	/// lerpTime 이 0 일 경우 즉시 변환합니다.
	class Action_OrnamentShow
		: public IAction
	{
		friend class editor::Module_ScriptEditor;

	public:
		DEFINE_ACTION(OrnamentShow)

		virtual ~Action_OrnamentShow();

		virtual CoroutineObject<void> DoAction() override;

		virtual bool IsValid() override;

		void SetTargetOrnament(editor::OrnamentData* ornament);
		void SetLerpTime(float lerpTime);

		virtual void ProcessObervationEvent(ObservationTarget* target, ObservationEvent event) override;

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

		virtual void ProgressInitialize() override;
		virtual void CurrentProgressSave() override;
		virtual void Recovery() override;

	private:
		editor::OrnamentData* targetOrnament = nullptr;
		float lerpTime = 0;
		bool isEditing = false;
		bool savedActive = true;
	};

	/// Ornament 의 Active 를 false 로 서서히 바꾸는 Action 입니다.
	/// lerpTime 이 0 일 경우 즉시 변환합니다.
	class Action_OrnamentHide
		: public IAction
	{
		friend class editor::Module_ScriptEditor;

	public:
		DEFINE_ACTION(OrnamentHide)

		virtual ~Action_OrnamentHide();

		virtual CoroutineObject<void> DoAction() override;

		virtual bool IsValid() override;

		void SetTargetOrnament(editor::OrnamentData* ornament);
		void SetLerpTime(float lerpTime);

		virtual void ProcessObervationEvent(ObservationTarget* target, ObservationEvent event) override;

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

		virtual void ProgressInitialize() override;
		virtual void CurrentProgressSave() override;
		virtual void Recovery() override;

	private:
		editor::OrnamentData* targetOrnament = nullptr;
		float lerpTime = 0;
		bool isEditing = false;
		bool savedActive = false;
	};

	/// Ornament 가 떠다니는 것처럼 만드는 Action 입니다.
	/// 시작 위치에서 입력한 거리 만큼 +y 축으로 진동합니다.
	class Action_OrnamentFloating
		: public IAction
	{
		friend class editor::Module_ScriptEditor;

	public:
		DEFINE_ACTION(OrnamentFloating)

		virtual ~Action_OrnamentFloating();

		virtual CoroutineObject<void> DoAction() override;

		virtual bool IsValid() override;

		void SetTargetOrnament(editor::OrnamentData* ornament);
		void SetDistance(float distance);

		/// 왕복 운동 시간입니다.
		/// 0 일 경우, Action 을 수행하지 않습니다.
		void SetRoundTripTime(float roundTripTime);
		void SetDirection(bool isUp);

		virtual void ProcessObervationEvent(ObservationTarget* target, ObservationEvent event) override;

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

		virtual void ProgressInitialize() override;
		virtual void CurrentProgressSave() override;
		virtual void Recovery() override;

	private:
		editor::OrnamentData* targetOrnament = nullptr;
		yunuGI::Vector3 scale = yunuGI::Vector3(1, 1, 1);
		float distance = 0;
		float roundTripTime = 1;
		bool isUpDirection = true;
		bool isEditing = false;
		bool prevCoroBreak = false;
	};
}
