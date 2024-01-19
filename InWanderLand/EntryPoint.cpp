#include "InWanderLand.h"
#include "Application.h"

// Main code
int main(int argc, char** argv)
{
	application::Application& client = application::Application::CreateApplication(argc, argv);
	client.Initialize();
	client.Run();
	client.Finalize();

	return 0;
}
