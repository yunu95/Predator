#include "ProgramExitCommand.h"

#include "Application.h"

namespace Application
{
	namespace Editor
	{
		ProgramExitCommand::ProgramExitCommand(Application& app)
			: app(app)
		{

		}

		void ProgramExitCommand::Execute()
		{
			app.TurnOff();
		}
	}
}
