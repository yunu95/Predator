/// 2023. 11. 23 김상준
/// EditableData 의 구체화된 클래스
/// 지형

#pragma once

#include "IEditableData.h"
#include "Terrain_InstanceData.h"
#include "Terrain_TemplateData.h"

#include <memory>
#include <string>

namespace Application
{
	namespace Editor
	{
		class Terrain
			: public IEditableData
		{
			friend class InstanceManager;

		public:
			virtual std::shared_ptr<IEditableData> Clone() const override;

			Terrain_InstanceData instanceData;
			std::shared_ptr<Terrain_TemplateData> templateData;

		protected:
			virtual bool PreEncoding(json& data) const override;
			virtual bool PostEncoding(json& data) const override;
			virtual bool PreDecoding(const json& data) override;
			virtual bool PostDecoding(const json& data) override;

		private:
			Terrain(const std::string& name);
			Terrain(const Terrain& prototype);
			Terrain& operator=(const Terrain& prototype);
		};
	}
}
