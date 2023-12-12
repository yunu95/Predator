#include "ProgramExitCommand.h"

#include "Application.h"

namespace Application
{
	namespace Editor
	{
		ProgramExitCommand::ProgramExitCommand()
			: app(Application::GetInstance())
		{

		}

		void ProgramExitCommand::Execute()
		{
			app.TurnOff();
		}
	}
}
