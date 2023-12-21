/// 2023. 12. 04 김상준
/// ITemplateData 에 대한 생성 및 관리를 위한 매니저 클래스

#pragma once

#include "Singleton.h"
#include "Storable.h"
#include "Identifiable.h"
#include "IEditableData.h"
#include "ITemplateData.h"

#include <memory>
#include <string>
#include <map>
#include <unordered_map>

namespace application
{
	namespace editor
	{
		class TemplateDataManager
			: public Storable, public Singleton<TemplateDataManager>
		{
		public:
			friend class MapFileManager;

			TemplateDataManager();

			ITemplateData* CreateTemplateData(const std::string& name, const IEditableData::DataType& type);
			ITemplateData* CloneTemplateData(const std::string& name, const ITemplateData* prototype);
			bool DeleteTemplateData(const std::string& name);
			ITemplateData* GetTemplateData(const std::string& name) const;
			IEditableData::DataType GetDataType(const std::string& name) const;
			IEditableData::DataType GetDataType(const ITemplateData* ptr) const;
			IEditableData::DataType GetDataType(const UUID& uuid) const;
			std::string GetDataKey(const ITemplateData* ptr) const;
			std::string GetDataKey(const UUID& uuid) const;
			void Clear();

		protected:
			virtual bool PreEncoding(json& data) const override;
			virtual bool PostEncoding(json& data) const override;
			virtual bool PreDecoding(const json& data) override;
			virtual bool PostDecoding(const json& data) override;

		private:
			std::map<const std::string, std::unique_ptr<ITemplateData>> list;
			std::unordered_map<const ITemplateData*, IEditableData::DataType> typeMap;
			std::unordered_map<const UUID, std::string> uuidKeyMap;
			std::unordered_map<const ITemplateData*, std::string> ptrKeyMap;
		};
	}
}
