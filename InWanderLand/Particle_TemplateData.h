/// 2024. 04. 23 김상준
/// ITemplateData 의 구체화된 클래스
/// 파티클

#pragma once

#include "ITemplateData.h"
#include "PodStructs.h"
#include "GlobalConstant.h"

#include <string>
#include <vector>

namespace application
{
	namespace editor
	{
		class Particle_TemplateData;

		enum class ParticleType
		{
			None = 0,
			Cone,
			Sphere,
		};

		struct POD_Particle_TemplateData
		{
			/// Particle 관련 데이터들
			ParticleType type = ParticleType::None;

			/// GlobalConstant



			TO_JSON(POD_Particle_TemplateData)
			FROM_JSON(POD_Particle_TemplateData)
		};

		class Particle_TemplateData
			:public ITemplateData
		{
			friend class ParticleData;
			friend class TemplateDataManager;

		public:
			virtual ~Particle_TemplateData();

			virtual std::string GetDataKey() const override;
			virtual void SetDataResourceName(std::string fbxName) override;
			virtual std::string GetDataResourceName() const override;
			virtual bool EnterDataFromGlobalConstant() override;

			POD_Particle_TemplateData pod;

		protected:
			virtual bool PreEncoding(json& data) const override;
			virtual bool PostEncoding(json& data) const override;
			virtual bool PreDecoding(const json& data) override;
			virtual bool PostDecoding(const json& data) override;

		private:
			Particle_TemplateData();
			Particle_TemplateData(const Particle_TemplateData& prototype);
			Particle_TemplateData& operator=(const Particle_TemplateData& prototype);
		};
	}
}

