/// 2023. 11. 23 김상준
/// 실제 Instance 가 되어 관리되는 EditableData 에 대한 Manager 클래스
/// EditableData 는 InstanceMaanger 를 통해서만 생성할 수 있음

#pragma once

#include "Storable.h"
#include "IEditableData.h"
#include "Identifiable.h"

#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>


namespace Application
{
	namespace Editor
	{
		class TemplateData;
	}
}

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

			// 이미 만들어진 TemplateData 로부터 Instance 를 생성함
			IEditableData* CreateInstance(const std::string& dataName);
			IEditableData* GetInstance(const UUID& uuid) const;
			void Clear();

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

			// TemplateData 가 생성되기 전에 그 틀만 우선 작성함
			IEditableData* CreateEmptyInstance(const IEditableData::DataType& type);

			std::unordered_map<const UUID, std::unique_ptr<IEditableData>> list;
			std::unordered_map<const UUID, TemplateData*> tdMap;
			// CreateEmptyInstance Only
			std::unordered_set<IEditableData*> listBeforeMatching;
			IEditableData* mould;
		};
	}
}


