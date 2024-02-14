#include "InWanderLand.h"
#include "UnitData.h"
#include "UnitPalette.h"
#include "UnitEditorInstance.h"

#include "InstanceManager.h"
#include "TemplateDataManager.h"
#include "MagicianProductor.h"
#include "WarriorProductor.h"
#include "HealerProductor.h"

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
			pod.x = newLoc.x;
			pod.y = newLoc.y;
			pod.z = newLoc.z;
		};

		palette::PaletteInstance* UnitData::ApplyAsPaletteInstance()
		{
			if (GetPaletteInstance() == nullptr)
			{
				unitInstance = Scene::getCurrentScene()->AddGameObject()->AddComponent<palette::UnitEditorInstance>();
				SetPaletteInstance(unitInstance);
				unitInstance->SetEditableData(this);
				unitInstance->Init(this);
			}
			unitInstance->GetTransform()->SetWorldPosition({ pod.x,pod.y,pod.z });
			return unitInstance;
		};
		void UnitData::ApplyAsPlaytimeObject()
		{
			// 함정과 같은 특수 기믹 객체들도 유닛과 유사하게 위치를 지정해주면 되기 때문에 UnitType에 Bomb, Trap, Bbang/th같은
			// 타입을 확장하여 유닛 생성 로직에서 같이 처리할 수 있게 만들 수 있다.
			pod.templateData->pod.m_atkRadius;
			UnitProductor* productionFactory{ nullptr };
			Vector3d position{ pod.x, pod.y, pod.z };
			// 유닛을 생성할 때 쓰일 팩토리를 먼저 지정
			switch (static_cast<Unit::UnitType>(pod.templateData->pod.unitType))
			{
				case Unit::UnitType::Healer:
					productionFactory = &HealerProductor::Instance();
					break;
				case Unit::UnitType::Warrior:
					productionFactory = &WarriorProductor::Instance();
					break;
				case Unit::UnitType::Magician:
					productionFactory = &MagicianProductor::Instance();
					break;
				default:
					break;
			}
			// 팩토리의 유닛 스펙을 에디터에서 저장한 데이터로 바꿈(유닛 체력, 공격속도 등등...)
			productionFactory->m_unitSpeed = pod.templateData->pod.m_unitSpeed;
			// 팩토리로 유닛을 에디터에서 저장된 위치에 생성
			productionFactory->CreateUnit(position);
			// 하지만 이 코드도 수정되어야 함. 유닛 데이터가 바로 유닛 생성으로 이어지는게 아니라,
			// 웨이브에 속하지 않은 유닛은 UnitSpawnPoint로, 웨이브에 속한 유닛은 따로 처리되어야 하기 때문.
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
