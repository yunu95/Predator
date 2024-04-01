/// 2024. 03. 29 김상준
/// Transform 에 한정하여 Ctrl + Z 기능을 지원하기 위한
/// Transform 수정 Command

#pragma once

#include "Command.h"
#include "YunutyEngine.h"
#include "IEditableData.h"

#include <vector>
#include <tuple>

namespace application
{
	namespace editor
	{
		struct TransformData
		{
			TransformData() = default;
			TransformData(const Transform* const ts)
			{
				this->position = ts->GetWorldPosition();
				this->rotation = ts->GetWorldRotation();
				this->scale = ts->GetWorldScale();
			}

			Vector3d position = Vector3d();
			Quaternion rotation = Quaternion();
			Vector3d scale = Vector3d::one;

			TransformData& operator= (const TransformData& data)
			{
				this->position = data.position;
				this->rotation = data.rotation;
				this->scale = data.scale;
				return *this;
			}
		};

		class TransformEditCommand
			: public UndoableCommand
		{
			/// 원래는 해당 방식으로 처리하면 안되지만,
			/// 임시로 Delete 에 대해서 직접 접근을 허용하여 처리하도록 함
			friend class DeleteInstanceCommand;

		public:
			COMMAND_SETTING(CommandType::EditTransform)

			virtual ~TransformEditCommand() = default;

			/// 첫 번째 TransformData 가 초기 위치, 두 번째 TransformData 가 변경된 위치
			TransformEditCommand(const std::vector<std::tuple<IEditableData*, TransformData, TransformData>>& list);

			virtual void Execute() override;
			virtual void Undo() override;

		private:
			std::vector<std::tuple<IEditableData*, TransformData, TransformData>> transformList;
		};
	}
}

