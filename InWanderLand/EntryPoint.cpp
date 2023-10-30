#include "Application.h"

// Main code
int main(int argc, char** argv)
{
	Application::Application& client = Application::Application::CreateApplication(argc, argv);
	client.Initialize();
	client.Run();
	client.Finalize();

	return 0;
}
