/// 2023. 11. 23 김상준
/// 실제 Instance 가 되어 관리되는 IEditableData 에 대한 Manager 클래스
/// IEditableData 는 InstanceMaanger 를 통해서만 생성할 수 있음

#pragma once

#include "Singleton.h"
#include "Storable.h"
#include "IEditableData.h"
#include "Identifiable.h"

#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>


namespace application
{
	namespace editor
	{
		class TemplateDataManager;
		class ITemplateData;
	}
}

namespace application
{
	namespace editor
	{
		class InstanceManager
			: public Storable, public Singleton<InstanceManager>
		{
			friend class Singleton<InstanceManager>;
			friend class MapFileManager;

		public:
			// 이미 만들어진 ITemplateData 로부터 Instance 를 생성함
			IEditableData* CreateInstance(const std::string& dataName);
			bool DeleteInstance(const UUID& uuid);
			IEditableData* GetInstance(const UUID& uuid) const;
			void Clear();

		protected:
			virtual bool PreEncoding(json& data) const override;
			virtual bool PostEncoding(json& data) const override;
			virtual bool PreDecoding(const json& data) override;
			virtual bool PostDecoding(const json& data) override;

		private:
			InstanceManager();

			// ITemplateData 가 생성되기 전에 그 틀만 우선 작성함
			IEditableData* CreateEmptyInstance(const DataType& type);

			TemplateDataManager& templateDataManager;
			std::unordered_map<const UUID, std::unique_ptr<IEditableData>> list;
			std::unordered_map<const UUID, ITemplateData*> tdMap;
			// CreateEmptyInstance Only
			std::unordered_set<IEditableData*> listBeforeMatching;
			IEditableData* mould;
		};
	}
}


