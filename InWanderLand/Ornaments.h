/// 2023. 11. 23 김상준
/// EditableData 의 구체화된 클래스
/// 장식물

#pragma once

#include "EditableData.h"
#include "Ornaments_InstanceData.h"
#include "Ornaments_SharedData.h"

#include <memory>

namespace Application
{
	namespace Editor
	{
		class Ornaments
			: public EditableData
		{
			friend class InstanceManager;

		public:
			virtual bool PreEncoding(json& data) const override;
			virtual bool PostEncoding(json& data) const override;
			virtual bool PreDecoding(const json& data) override;
			virtual bool PostDecoding(const json& data) override;

			virtual std::shared_ptr<EditableData> Clone() override;

			Ornaments_InstanceData instanceData;
			std::shared_ptr<Ornaments_SharedData>& sharedData;

		private:
			Ornaments();
			Ornaments(const Ornaments& prototype);
			Ornaments& operator=(const Ornaments& prototype);
		};
	}
}
