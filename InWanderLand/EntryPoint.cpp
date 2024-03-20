#include "InWanderLand.h"
#include "Application.h"
#include "WarriorProductor.h"
#include "MagicianProductor.h"
#include "HealerProductor.h"
#include "DebugMeshes.h"
#include "RTSCam.h"


// Main code
int main(int argc, char** argv)
//int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	application::Application& client = application::Application::CreateApplication(argc, argv);
	client.Initialize();
	client.Run();
	client.Finalize();

	return 0;
}
