/// 2024. 04. 17 김상준
/// Camera 에 관련된 Action 목록입니다. 

#pragma once

#include "IAction.h"

#include "PlayableComponent.h"

#include "YunuGraphicsInterface.h"

namespace application
{
	namespace editor
	{
		class Module_ScriptEditor;
		class CameraData;
	}

	/// 특정 카메라 View 로 변경하는 Action 입니다.
	class Action_CameraChangeView
		: public IAction
	{
		friend class editor::Module_ScriptEditor;

	public:
		DEFINE_ACTION(CameraChangeView)

		virtual ~Action_CameraChangeView();

		virtual CoroutineObject<void> DoAction() override;

		virtual bool IsValid() override;

		void SetCamera(editor::CameraData* cam);
		void SetLerpTime(float lerpTime);

		virtual void ProcessObervationEvent(ObservationTarget* target, ObservationEvent event) override;

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

	private:
		editor::CameraData* targetCam = nullptr;
		bool isEditing = false;
		float lerpTime = 0;
	};

	class Action_CameraLoadView;

	/// 현재 시점의 MainCamera 의 View 를 저장합니다.
	class Action_CameraSaveView
		: public IAction
	{
		friend class editor::Module_ScriptEditor;
		friend class Action_CameraLoadView;

	public:
		DEFINE_ACTION(CameraSaveView)

		virtual CoroutineObject<void> DoAction() override;

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

		static void Clear();

	private:
		static float savedFov;
		static float savedCameraNear;
		static float savedCameraFar;
		static float savedWidth;
		static float savedHeight;
		static yunuGI::Vector3 savedPosition;
		static yunuGI::Quaternion savedRotation;
		static yunuGI::Vector3 savedScale;
	};

	/// 저장된 MainCamera 의 View 를 적용합니다.
	/// 저장된 MainCamera 의 View 가 없을 경우, 아무 일도 일어나지 않습니다.
	class Action_CameraLoadView
		: public IAction
	{
		friend class editor::Module_ScriptEditor;

	public:
		DEFINE_ACTION(CameraLoadView)

		virtual CoroutineObject<void> DoAction() override;

		void SetLerpTime(float lerpTime);

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

	private:
		float lerpTime = 0;
	};

	/// 초기 상태의 게임 카메라 위치로 되돌립니다.
	class Action_CameraRevert
		: public IAction
	{
		friend class editor::Module_ScriptEditor;

	public:
		DEFINE_ACTION(CameraRevert)

		virtual CoroutineObject<void> DoAction() override;

		void SetLerpTime(float lerpTime);

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

	private:
		float lerpTime = 0;
	};

	/// 전술모드에서 사용할 카메라를 세팅합니다.
	class Action_SetTacticCamera
		: public IAction
	{
		friend class editor::Module_ScriptEditor;

	public:
		DEFINE_ACTION(SetTacticCamera)

		virtual ~Action_SetTacticCamera();

		virtual CoroutineObject<void> DoAction() override;

		virtual bool IsValid() override;

		void SetCamera(editor::CameraData* cam);

		virtual void ProcessObervationEvent(ObservationTarget* target, ObservationEvent event) override;

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

	private:
		editor::CameraData* targetCam = nullptr;
		bool isEditing = false;
	};
}
