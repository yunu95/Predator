/// 2023. 12. 04 김상준
/// TemplateData 에 대한 생성 및 관리를 위한 매니저 클래스

#pragma once

#include "Storable.h"
#include "Identifiable.h"
#include "IEditableData.h"
#include "TemplateData.h"

#include <memory>
#include <string>
#include <map>
#include <unordered_map>

namespace Application
{
	namespace Editor
	{
		class TemplateDataManager
			: public Storable
		{
		public:
			static TemplateDataManager& GetInstance();

			virtual ~TemplateDataManager();

			TemplateData* CreateTemplateData(const std::string& name, const IEditableData::DataType& type);
			TemplateData* CloneTemplateData(const std::string& name, const TemplateData* prototype);
			bool DeleteTemplateData(const std::string& name);
			TemplateData* GetTemplateData(const std::string& name) const;
			IEditableData::DataType GetDataType(const std::string& name) const;
			IEditableData::DataType GetDataType(const TemplateData* ptr) const;
			IEditableData::DataType GetDataType(const UUID& uuid) const;
			std::string GetDataKey(const TemplateData* ptr) const;
			std::string GetDataKey(const UUID& uuid) const;
			void Clear();

		protected:
			virtual bool PreEncoding(json& data) const override;
			virtual bool PostEncoding(json& data) const override;
			virtual bool PreDecoding(const json& data) override;
			virtual bool PostDecoding(const json& data) override;

		private:
			static std::unique_ptr<TemplateDataManager> instance;

			TemplateDataManager();
			TemplateDataManager(const TemplateDataManager&) = delete;
			TemplateDataManager& operator=(const TemplateDataManager&) = delete;

			std::map<const std::string, std::unique_ptr<TemplateData>> list;
			std::unordered_map<const TemplateData*, IEditableData::DataType> typeMap;
			std::unordered_map<const UUID, std::string> uuidKeyMap;
			std::unordered_map<const TemplateData*, std::string> ptrKeyMap;
		};
	}
}
