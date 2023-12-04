/// 2023. 11. 23 김상준
/// 실제 Instance 가 되어 관리되는 EditableData 에 대한 Manager 클래스
/// EditableData 는 InstanceMaanger 를 통해서만 생성할 수 있음

#pragma once

#include "EditableData.h"

#include <string>
#include <memory>
#include <set>

namespace Application
{
	namespace Editor
	{
		class InstanceManager
		{
		public:
			static InstanceManager& GetInstance();

			virtual ~InstanceManager();

			bool CreateInstance(const std::string& dataName);
			bool CloneInstance(const std::shared_ptr<EditableData>& prototype);
			bool Save(json& jsonData);
			bool Load(const json& jsonData);

		private:
			static std::unique_ptr<InstanceManager> instance;

			InstanceManager();
			InstanceManager(const InstanceManager&) = delete;
			InstanceManager& operator=(const InstanceManager&) = delete;

			void Clear();

			std::set<std::shared_ptr<EditableData>> list;
			std::shared_ptr<EditableData> mould;
		};
	}
}


