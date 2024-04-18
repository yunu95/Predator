#include "TransformEditCommand.h"

#include "YunutyEngine.h"

namespace application
{
	namespace editor
	{
		TransformEditCommand::~TransformEditCommand()
		{
			for (auto& [key, start, end] : transformList)
			{
				key->RemoveObserver(this);
			}
		}

		TransformEditCommand::TransformEditCommand(const std::vector<std::tuple<IEditableData*, TransformData, TransformData>>& list)
		{
			transformList = list;

			for (auto& [key, start, end] : transformList)
			{
				key->RegisterObserver(this);
			}
		}

		void TransformEditCommand::Execute()
		{
			for (auto& [key, start, end] : transformList)
			{
				if (key == nullptr)
				{
					continue;
				}

				key->OnRelocate(end.position);
				key->OnRerotate(end.rotation);
				key->OnRescale(end.scale);
				key->ApplyAsPaletteInstance();
			}
		}

		void TransformEditCommand::Undo()
		{
			for (auto& [key, start, end] : transformList)
			{
				if (key == nullptr)
				{
					continue;
				}

				key->OnRelocate(start.position);
				key->OnRerotate(start.rotation);
				key->OnRescale(start.scale);
				key->ApplyAsPaletteInstance();
			}
		}

		void TransformEditCommand::ProcessObervationEvent(ObservationTarget* target, ObservationEvent event)
		{
			if (target)
			{
				switch (event)
				{
					case application::ObservationEvent::Destroy:
					{
						for (auto& [key, start, end] : transformList)
						{
							if (key == static_cast<IEditableData*>(target))
							{
								key = nullptr;
								break;
							}
						}
						break;
					}
					default:
						break;
				}
			}
		}
	}
}
