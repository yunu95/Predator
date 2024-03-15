#include "InWanderLand.h"
#include "UnitData.h"
#include "UnitPalette.h"
#include "UnitEditorInstance.h"

#include "InstanceManager.h"
#include "TemplateDataManager.h"
#include "UnitClassifier.h"
#include "Unit.h"
#include "UnitProductor.h"
#include "WarriorProductor.h"
#include "MagicianProductor.h"
#include "HealerProductor.h"
#include "MeleeEnemyProductor.h"

namespace application
{
	namespace editor
	{
		TemplateDataManager& UnitData::templateDataManager = TemplateDataManager::GetSingletonInstance();

		bool UnitData::EnterDataFromTemplate()
		{
			// 템플릿으로부터 초기화되는 데이터들 처리 영역	

			return true;
		}

		ITemplateData* UnitData::GetTemplateData()
		{
			return pod.templateData;
		}

		bool UnitData::SetTemplateData(const std::string& dataName)
		{
			auto ptr = templateDataManager.GetTemplateData(dataName);
			if (ptr == nullptr)
			{
				return false;
			}

			pod.templateData = static_cast<Unit_TemplateData*>(ptr);

			return true;
		}

		IEditableData* UnitData::Clone() const
		{
			auto& imanager = InstanceManager::GetSingletonInstance();
			auto instance = imanager.CreateInstance(pod.templateData->GetDataKey());

			if (instance != nullptr)
			{
				static_cast<UnitData*>(instance)->pod = pod;
			}

			return instance;
		}

		void UnitData::OnRelocate(const Vector3d& newLoc)
		{
			pod.position.x = newLoc.x;
			pod.position.y = newLoc.y;
			pod.position.z = newLoc.z;
		}

		void UnitData::OnRerotate(const Quaternion& newRot)
		{
			pod.rotation.x = newRot.x;
			pod.rotation.y = newRot.y;
			pod.rotation.z = newRot.z;
			pod.rotation.w = newRot.w;
		}

		void UnitData::OnRescale(const Vector3d& newScale)
		{
			pod.scale.x = newScale.x;
			pod.scale.y = newScale.y;
			pod.scale.z = newScale.z;
		}

		void UnitData::OnDataResourceChange(std::string newName)
		{
			SetTemplateData(newName);
			if (unitInstance)
			{
				unitInstance->ChangeTemplateData(this);
			}
		}

		palette::PaletteInstance* UnitData::ApplyAsPaletteInstance()
		{
			if (GetPaletteInstance() == nullptr)
			{
				unitInstance = Scene::getCurrentScene()->AddGameObject()->AddComponent<palette::UnitEditorInstance>();
				SetPaletteInstance(unitInstance);
				unitInstance->SetEditableData(this);
				unitInstance->Init(this);
			}
			unitInstance->GetTransform()->SetWorldPosition({ pod.position.x,pod.position.y,pod.position.z });
			unitInstance->GetTransform()->SetWorldRotation({ pod.rotation.w, pod.rotation.x,pod.rotation.y,pod.rotation.z });
			unitInstance->GetTransform()->SetLocalScale({ pod.scale.x,pod.scale.y,pod.scale.z });
			return unitInstance;
		};
		void UnitData::ApplyAsPlaytimeObject()
		{
			// 함정과 같은 특수 기믹 객체들도 유닛과 유사하게 위치를 지정해주면 되기 때문에 UnitType에 Bomb, Trap, Bbang/th같은
			// 타입을 확장하여 유닛 생성 로직에서 같이 처리할 수 있게 만들 수 있다.
			UnitClassifier::SingleInstance().SendPODToClassifier(pod);
			pod.waveData->pod.waveUnitUUIDS;
		}

		void UnitData::PostApplyAsPlaytimeObject()
		{
			/// 
			UnitProductor* currentSelectedProductor{ nullptr };

			switch (static_cast<Unit::UnitType>(pod.templateData->pod.unitType))
			{
				case Unit::UnitType::Healer:
					currentSelectedProductor = &HealerProductor::Instance();
					break;
				case Unit::UnitType::Warrior:
					currentSelectedProductor = &WarriorProductor::Instance();
					break;
				case Unit::UnitType::Magician:
					currentSelectedProductor = &MagicianProductor::Instance();
					break;
				default:
					currentSelectedProductor = &MeleeEnemyProductor::Instance();
					break;
			}

			currentSelectedProductor->MappingUnitData(pod.templateData->pod);
			pod.waveData->playtimeWave->m_productorVector.push_back(currentSelectedProductor);		// 이렇게 밀어넣어준다 해서 실제 wave 순서와 맞아 떨어지지 않음. 임시방편
		}

		bool UnitData::PreEncoding(json& data) const
		{
			FieldPreEncoding<boost::pfr::tuple_size_v<POD_Unit>>(pod, data["POD"]);

			return true;
		}

		bool UnitData::PostEncoding(json& data) const
		{
			FieldPostEncoding<boost::pfr::tuple_size_v<POD_Unit>>(pod, data["POD"]);

			return true;
		}

		bool UnitData::PreDecoding(const json& data)
		{
			FieldPreDecoding<boost::pfr::tuple_size_v<POD_Unit>>(pod, data["POD"]);

			return true;
		}

		bool UnitData::PostDecoding(const json& data)
		{
			FieldPostDecoding<boost::pfr::tuple_size_v<POD_Unit>>(pod, data["POD"]);
#ifdef EDITOR
			ApplyAsPaletteInstance();
#endif
			return true;
		}

		UnitData::UnitData()
			: pod()
		{

		}

		UnitData::UnitData(const std::string& name)
			: IEditableData(), pod()
		{
			pod.templateData = static_cast<Unit_TemplateData*>(templateDataManager.GetTemplateData(name));
			EnterDataFromTemplate();
		}

		UnitData::UnitData(const UnitData& prototype)
			: pod(prototype.pod)
		{

		}

		UnitData& UnitData::operator=(const UnitData& prototype)
		{
			IEditableData::operator=(prototype);
			pod = prototype.pod;
			return *this;
		}
	}
}
