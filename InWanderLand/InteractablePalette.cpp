#include "InteractablePalette.h"
#include "WavePalette.h"
#include "InteractableEditorInstance.h"
#include "SelectionBox.h"
#include "InstanceManager.h"
#include "Interactable_TemplateData.h"
#include "InteractableData.h"
#include "TemplateDataManager.h"
#include "InstanceManager.h"
#include "InteractableBrush.h"

namespace application::editor::palette
{
	InteractableData* InteractablePalette::GetSingleSelectedInteractable()
	{
		return selection.empty() ? nullptr : selection.size() != 1 ? nullptr : static_cast<InteractableData*>(const_cast<IEditableData*>(*selection.begin()));
	}

	void InteractablePalette::SelectInteractable(const UUID& interactableUUID)
	{
		auto ptr = InstanceManager::GetSingletonInstance().GetInstance(interactableUUID);
		if (ptr)
		{
			Palette::OnSelectSingleInstance(ptr);
		}
	}

	void InteractablePalette::SetMatchingMode(bool mode)
	{
		if (mode)
		{
			if (selection.size() == 1)
			{
				subjectData = static_cast<InteractableData*>(*selection.begin());
				matchingMode = mode;
			}
		}
		else
		{
			subjectData = nullptr;
			matchingMode = mode;
		}
	}

	void InteractablePalette::SelectInteractableTemplateData(Interactable_TemplateData* templateData)
	{
		selectedInteractableTemplateData = templateData;
		if (selectedInteractableTemplateData == nullptr)
		{
			InteractableBrush::Instance().ReadyBrush("");
		}
		else
		{
			InteractableBrush::Instance().ReadyBrush(selectedInteractableTemplateData->GetDataKey());
		}
	}

	void InteractablePalette::UnselectInteractableTemplateData()
	{
		SelectInteractableTemplateData(nullptr);
	}

	void InteractablePalette::Reset()
	{
		Palette::Reset();
		UnselectInteractableTemplateData();
		SetMatchingMode(false);
	}

	IEditableData* InteractablePalette::PlaceInstance(Vector3d worldPosition)
	{
		if (selectedInteractableTemplateData == nullptr)
			return nullptr;

		auto instance = InstanceManager::GetSingletonInstance().CreateInstance<InteractableData>(selectedInteractableTemplateData->GetDataKey());
		instance->pod.position.x = worldPosition.x;
		instance->pod.position.y = worldPosition.y;
		instance->pod.position.z = worldPosition.z;
		instance->ApplyAsPaletteInstance();
		return instance;
	}

	void InteractablePalette::OnMouseMove(Vector3d projectedWorldPos, Vector2d normalizedScreenPos)
	{
		Palette::OnMouseMove(projectedWorldPos, normalizedScreenPos);
		// 브러시 움직이기
		InteractableBrush::Instance().GetTransform()->SetWorldPosition(projectedWorldPos);
		if (IsClickingLeft() && !IsSelectMode() && CheckInstantiationCooltime())
			PlaceInstance(projectedWorldPos);
	}

	void InteractablePalette::OnDeletion()
	{
		if (selection.contains(subjectData))
		{
			return;
		}

		Palette::OnDeletion();
	}

	void InteractablePalette::SetAsSelectMode(bool isSelectMode)
	{
		Palette::SetAsSelectMode(isSelectMode);

		if (isSelectMode)
		{
			InteractableBrush::Instance().ReadyBrush("");
		}
		else
		{
			SelectInteractableTemplateData(selectedInteractableTemplateData);
		}
	}

	bool InteractablePalette::ShouldSelect(IEditableData* instance)
	{
		return dynamic_cast<InteractableData*>(instance);
	}

	void InteractablePalette::OnStartPalette()
	{
		switch (beforeState)
		{
			case application::editor::palette::Palette::State::Place:
			{
				SetAsSelectMode(false);
				break;
			}
			default:
				SetAsSelectMode(true);
				break;
		}
		InteractableBrush::Instance().GetGameObject()->SetSelfActive(true);
	}

	void InteractablePalette::OnStandbyPalette()
	{
		if (IsSelectMode())
		{
			beforeState = State::Select;
		}
		else
		{
			beforeState = State::Place;
		}
		InteractableBrush::Instance().GetGameObject()->SetSelfActive(false);
		CleanUpData();
	}
}
