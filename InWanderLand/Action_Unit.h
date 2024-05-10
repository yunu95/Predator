/// 2024. 05. 10 김상준
/// Unit 에 관련된 Action 목록입니다. 

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
		class UnitData;
	}

	/// 특정 위치로 Unit 의 상태를 이동하는 Action 입니다.
	class Action_UnitMove
		: public IAction
	{
		friend class editor::Module_ScriptEditor;

	public:
		DEFINE_ACTION(UnitMove)

		virtual ~Action_UnitMove();

		virtual CoroutineObject<void> DoAction() override;

		virtual bool IsValid() override;

		void SetUnit(editor::UnitData* unit);
		void SetDestinationUnit(editor::UnitData* unit);
		void SetLerpTime(float lerpTime);

		virtual void ProcessObervationEvent(ObservationTarget* target, ObservationEvent event) override;

		virtual bool PreEncoding(json& data) const override;
		virtual bool PostEncoding(json& data) const override;
		virtual bool PreDecoding(const json& data) override;
		virtual bool PostDecoding(const json& data) override;

	private:
		void UpdateDestinationFromContainer();
		void UpdatePosition(const yunuGI::Vector3& pos);
		void UpdateRotation(const yunuGI::Quaternion& rot);
		void UpdateScale(const yunuGI::Vector3& scal);

		editor::UnitData* targetUnit = nullptr;
		yunuGI::Vector3 position = yunuGI::Vector3();
		yunuGI::Quaternion rotation = yunuGI::Quaternion();
		yunuGI::Vector3 scale = yunuGI::Vector3(1, 1, 1);
		bool destinationSetting = false;
		yunuGI::Vector3 container_pos = yunuGI::Vector3();
		yunuGI::Vector3 container_rot = yunuGI::Vector3();
		yunuGI::Vector3 container_scal = yunuGI::Vector3(1, 1, 1);
		bool isEditing = false;
		float lerpTime = 0;
	};
}
