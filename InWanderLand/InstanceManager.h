/// 2023. 11. 23 김상준
/// 실제 Instance 가 되어 관리되는 EditableData 에 대한 Manager 클래스
/// EditableData 는 InstanceMaanger 를 통해서만 생성할 수 있음

#pragma once

#include "Storable.h"
#include "IEditableData.h"

#include <string>
#include <memory>
#include <unordered_set>

namespace Application
{
	namespace Editor
	{
		class InstanceManager
			: public Storable
		{
		public:
			static InstanceManager& GetInstance();

			virtual ~InstanceManager();

			bool CreateInstance(const std::string& dataName);
			bool CloneInstance(const std::shared_ptr<IEditableData>& prototype);

		protected:
			virtual bool PreEncoding(json& data) const override;
			virtual bool PostEncoding(json& data) const override;
			virtual bool PreDecoding(const json& data) override;
			virtual bool PostDecoding(const json& data) override;

		private:
			static std::unique_ptr<InstanceManager> instance;

			InstanceManager();
			InstanceManager(const InstanceManager&) = delete;
			InstanceManager& operator=(const InstanceManager&) = delete;

			void Clear();

			std::unordered_set<std::shared_ptr<IEditableData>> list;
			std::shared_ptr<IEditableData> mould;
		};
	}
}


