#include "ProgramExitCommand.h"

#include "Application.h"

namespace application
{
	namespace editor
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
