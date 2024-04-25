#include "InWanderLand.h"
#include "Particle_TemplateData.h"

#include "TemplateDataManager.h"
#include "InstanceManager.h"
#include "ParticleBrush.h"
#include "ParticleData.h"

namespace application
{
	namespace editor
	{
		Particle_TemplateData::~Particle_TemplateData()
		{

		}

		std::string Particle_TemplateData::GetDataKey() const
		{
			return TemplateDataManager::GetSingletonInstance().GetDataKey(this);
		}

		void Particle_TemplateData::SetDataResourceName(std::string fbxName)
		{
			return;
		}

		std::string Particle_TemplateData::GetDataResourceName() const
		{
			return std::string();
		}

		bool Particle_TemplateData::EnterDataFromGlobalConstant()
		{
			auto& data = GlobalConstant::GetSingletonInstance().pod;
			return true;
		}

		bool Particle_TemplateData::PreEncoding(json& data) const
		{
			FieldPreEncoding<boost::pfr::tuple_size_v<POD_Particle_TemplateData>>(pod, data["POD"]);

			return true;
		}

		bool Particle_TemplateData::PostEncoding(json& data) const
		{
			FieldPostEncoding<boost::pfr::tuple_size_v<POD_Particle_TemplateData>>(pod, data["POD"]);

			return true;
		}

		bool Particle_TemplateData::PreDecoding(const json& data)
		{
			FieldPreDecoding<boost::pfr::tuple_size_v<POD_Particle_TemplateData>>(pod, data["POD"]);

			return true;
		}

		bool Particle_TemplateData::PostDecoding(const json& data)
		{
			FieldPostDecoding<boost::pfr::tuple_size_v<POD_Particle_TemplateData>>(pod, data["POD"]);
			EnterDataFromGlobalConstant();
			return true;
		}

		Particle_TemplateData::Particle_TemplateData()
			: ITemplateData(), pod()
		{
			EnterDataFromGlobalConstant();
		}

		Particle_TemplateData::Particle_TemplateData(const Particle_TemplateData& prototype)
			: ITemplateData(prototype), pod(prototype.pod)
		{
			EnterDataFromGlobalConstant();
		}

		Particle_TemplateData& Particle_TemplateData::operator=(const Particle_TemplateData& prototype)
		{
			ITemplateData::operator=(prototype);
			pod = prototype.pod;
			return *this;
		}
	}
}
