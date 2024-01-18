/// 2023. 12. 04 김상준
/// ITemplateData 에 대한 생성 및 관리를 위한 매니저 클래스

#pragma once

#ifdef EDITOR
#include "Singleton.h"
#include "Storable.h"
#include "Identifiable.h"
#include "IEditableData.h"
#include "ITemplateData.h"

#include <memory>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>

namespace application
{
	namespace editor
	{
		class TemplateDataManager
			: public Storable, public Singleton<TemplateDataManager>
		{
			friend class Singleton<TemplateDataManager>;
			friend class MapFileManager;

		public:
			ITemplateData* CreateTemplateData(const std::string& name, const DataType& type);
			ITemplateData* CloneTemplateData(const std::string& name, const ITemplateData* prototype);
			bool DeleteTemplateData(const std::string& name);
			ITemplateData* GetTemplateData(const std::string& name) const;
			DataType GetDataType(const std::string& name) const;
			DataType GetDataType(const ITemplateData* ptr) const;
			DataType GetDataType(const UUID& uuid) const;
			std::string GetDataKey(const ITemplateData* ptr) const;
			std::string GetDataKey(const UUID& uuid) const;
			const std::vector<ITemplateData*>& GetDataList(const DataType& type);
			void Clear();

		protected:
			virtual bool PreEncoding(json& data) const override;
			virtual bool PostEncoding(json& data) const override;
			virtual bool PreDecoding(const json& data) override;
			virtual bool PostDecoding(const json& data) override;

		private:
			TemplateDataManager();

			std::map<const std::string, std::unique_ptr<ITemplateData>> list;
			std::unordered_map<const ITemplateData*, DataType> typeMap;
			std::unordered_map<const UUID, std::string> uuidKeyMap;
			std::unordered_map<const ITemplateData*, std::string> ptrKeyMap;
			std::vector<ITemplateData*> dataContainer;
		};
	}
}
#endif
