#include "YunutyEngine.h"

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "ImGuizmo/ImGuizmo.h"
#include <d3d11.h>
#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <dxgi1_4.h>

bool g_preLoad = false;
std::vector<yunutyEngine::GameObject*> g_gameObjVec;

// Data
static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain1* g_pSwapChain = nullptr;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

HWND g_hwnd = nullptr;
HWND g_Toolhwnd = nullptr;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param);
LRESULT CALLBACK WndProcTool(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param);

void CreateMyWindow(HINSTANCE h_instance, HINSTANCE h_prev_instance, LPSTR lp_cmd_line, int n_cmd_show);
void CreateToolWindow(HINSTANCE h_instance, HINSTANCE h_prev_instance, LPSTR lp_cmd_line, int n_cmd_show);

void PreLoadResource();
void CreateObject(std::string fbxName);
void ShowGameObject(yunutyEngine::GameObject* obj);
void ShowFBXList();


int WINAPI main(HINSTANCE h_instance, HINSTANCE h_prev_instance, LPSTR lp_cmd_line, int n_cmd_show)
{
	CreateMyWindow(h_instance, h_prev_instance, lp_cmd_line, n_cmd_show);

	CreateToolWindow(h_instance, h_prev_instance, lp_cmd_line, n_cmd_show);

	yunutyEngine::graphics::Renderer::SingleInstance().LoadGraphicsDll(L"NailEngine.dll");
	yunutyEngine::graphics::Renderer::SingleInstance().SetResolution(1920, 1080);
	yunutyEngine::graphics::Renderer::SingleInstance().SetOutputWindow(g_hwnd);

	// Setup Platform/Renderer backends
	g_pd3dDevice = reinterpret_cast<ID3D11Device*>(yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager()->GetDevice());
	g_pd3dDeviceContext = reinterpret_cast<ID3D11DeviceContext*>(yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager()->GetDeviceContext());

	// Initialize Direct3D
	if (!CreateDeviceD3D(g_Toolhwnd))
	{
		CleanupDeviceD3D();
		return 1;
	}


	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
	io.DisplaySize = ImVec2(1920, 1080);
	/// Custom 영역
	// 타이틀 바를 컨트롤 할 때에만 움직임
	io.ConfigWindowsMoveFromTitleBarOnly = true;
	///

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(g_Toolhwnd);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

	yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());

	static bool isRunning = true;
	while (isRunning)
	{
		MSG msg;
		while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
			{
				isRunning = false;
			}

			ImGui_ImplWin32_WndProcHandler(msg.hwnd, msg.message, msg.wParam, msg.lParam);
		}

		if (g_preLoad)
		{
			continue;
		}

		// Start the Dear ImGui frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		bool show_demo_window = true;
		bool show_another_window = false;
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

		{
			static float f = 0.0f;
			static int counter = 0;
			ImGui::Begin("Buttons");                          // Create a window called "Hello, world!" and append into it.

			if (ImGui::Button("SaveButton"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			{
				counter++;
			}

			if (ImGui::Button("LoadButton"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			{
				counter++;
			}

			if (ImGui::Button("PreLoadButton"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			{
				PreLoadResource();
			}

			ImGui::End();



			ImGui::Begin("FBXList");

			ShowFBXList();

			ImGui::End();
			



			ImGui::Begin("Info");


			ImGui::End();
		}

		// Rendering
		ImGui::Render();
		const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
		g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
		g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		g_pSwapChain->Present(1, 0); // Present with vsync	

		// Update platform windows (should be called after rendering)
		ImGui::UpdatePlatformWindows();
	}



	return 0; // 성공적으로 종료
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) {
	switch (message) {
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hwnd, message, w_param, l_param);
	}
	return 0;
}

LRESULT CALLBACK WndProcTool(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) {
	switch (message) {
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hwnd, message, w_param, l_param);
	}
	return 0;
}

// Helper functions to use DirectX11
bool CreateDeviceD3D(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC1 sd{};
	sd.BufferCount = 2;
	sd.Width = 0;
	sd.Height = 0;
	sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.Stereo = FALSE;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.Scaling = DXGI_SCALING_STRETCH;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	IDXGIDevice2* dxgiDevice;
	auto result1 = g_pd3dDevice->QueryInterface(IID_PPV_ARGS(&dxgiDevice));

	IDXGIAdapter* dxgiAdapter;
	auto result2 = dxgiDevice->GetAdapter(&dxgiAdapter);

	IDXGIFactory2* dxgiFactory = nullptr;
	auto result3 = dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));
	dxgiAdapter->Release();

	auto result4 = dxgiFactory->CreateSwapChainForHwnd(g_pd3dDevice, hWnd, &sd, nullptr, nullptr, &g_pSwapChain);
	dxgiFactory->Release();

	CreateRenderTarget();
	return true;
}

void CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
	if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget()
{
	ID3D11Texture2D* pBackBuffer;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
	pBackBuffer->Release();
}

void CleanupRenderTarget()
{
	if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

void CreateMyWindow(HINSTANCE h_instance, HINSTANCE h_prev_instance, LPSTR lp_cmd_line, int n_cmd_show)
{
	// 윈도우 클래스 등록
	WNDCLASS wc = {};
	wc.lpfnWndProc = WndProc;
	wc.hInstance = h_instance;
	wc.lpszClassName = L"MyWindowClass";
	RegisterClass(&wc);

	// 윈도우 생성
	g_hwnd = CreateWindow(
		L"MyWindowClass",   // 등록한 윈도우 클래스 이름
		L"My Window",       // 윈도우 제목
		WS_OVERLAPPEDWINDOW, // 윈도우 스타일
		100,      // x 좌표
		100,      // y 좌표
		1920,                // 너비
		1080,                // 높이
		nullptr,            // 부모 윈도우
		nullptr,            // 메뉴 핸들
		h_instance,         // 인스턴스 핸들
		nullptr             // 추가 파라미터
	);

	// 윈도우를 화면에 표시
	ShowWindow(g_hwnd, SW_SHOWDEFAULT);
	UpdateWindow(g_hwnd);


}


void CreateToolWindow(HINSTANCE h_instance, HINSTANCE h_prev_instance, LPSTR lp_cmd_line, int n_cmd_show)
{
	// 윈도우 클래스 등록
	WNDCLASSEX wcEditor = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProcTool, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ResourceTool"), NULL };
	RegisterClassEx(&wcEditor);

	// 윈도우 생성
	g_Toolhwnd = ::CreateWindow(wcEditor.lpszClassName, wcEditor.lpszClassName, WS_OVERLAPPEDWINDOW, 2020, 100, 1920, 1080, g_hwnd, NULL, wcEditor.hInstance, NULL);

	// 윈도우를 화면에 표시
	ShowWindow(g_Toolhwnd, SW_SHOWDEFAULT);
	UpdateWindow(g_Toolhwnd);

}

void PreLoadResource()
{
	g_preLoad = true;

	const yunuGI::IResourceManager* resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
	resourceManager->LoadFile("FBX/SM_Bush_001");
	CreateObject("SM_Bush_001");
	resourceManager->LoadFile("FBX/SM_Bush_002");
	CreateObject("SM_Bush_002");
	resourceManager->LoadFile("FBX/SM_CastleWall");
	CreateObject("SM_CastleWall");
	resourceManager->LoadFile("FBX/SM_CastleWall_Door");
	CreateObject("SM_CastleWall_Door");
	resourceManager->LoadFile("FBX/SM_CastleWall_Pillar");
	CreateObject("SM_CastleWall_Pillar");
	resourceManager->LoadFile("FBX/SM_Chair");
	CreateObject("SM_Chair");
	resourceManager->LoadFile("FBX/SM_Cuptower");
	CreateObject("SM_Cuptower");
	resourceManager->LoadFile("FBX/SM_Fork");
	CreateObject("SM_Fork");
	resourceManager->LoadFile("FBX/SM_GuideBook");
	CreateObject("SM_GuideBook");
	resourceManager->LoadFile("FBX/SM_Hat01");
	CreateObject("SM_Hat01");
	resourceManager->LoadFile("FBX/SM_Hat02");
	CreateObject("SM_Hat02");
	resourceManager->LoadFile("FBX/SM_SmallBush_001");
	CreateObject("SM_SmallBush_001");
	resourceManager->LoadFile("FBX/SM_Stone_001");
	CreateObject("SM_Stone_001");
	resourceManager->LoadFile("FBX/SM_Stone_002");
	CreateObject("SM_Stone_002");
	resourceManager->LoadFile("FBX/SM_Stump");
	CreateObject("SM_Stump");
	resourceManager->LoadFile("FBX/SM_Temple_Book_etc");
	CreateObject("SM_Temple_Book_etc");
	resourceManager->LoadFile("FBX/SM_Temple_Books");
	CreateObject("SM_Temple_Books");
	resourceManager->LoadFile("FBX/SM_Temple_Floor");
	CreateObject("SM_Temple_Floor");
	resourceManager->LoadFile("FBX/SM_Temple_Pillar");
	CreateObject("SM_Temple_Pillar");
	resourceManager->LoadFile("FBX/SM_Temple_Pillar_Broken");
	CreateObject("SM_Temple_Pillar_Broken");
	resourceManager->LoadFile("FBX/SM_Temple_Rabbit");
	CreateObject("SM_Temple_Rabbit");
	resourceManager->LoadFile("FBX/SM_Temple_Stairs");
	CreateObject("SM_Temple_Stairs");
	resourceManager->LoadFile("FBX/SM_Temple_Welcome");
	CreateObject("SM_Temple_Welcome");
	resourceManager->LoadFile("FBX/SM_Trunk_001");
	CreateObject("SM_Trunk_001");


	g_preLoad = false;
}

void CreateObject(std::string fbxName)
{
	g_gameObjVec.emplace_back(Scene::getCurrentScene()->AddGameObjectFromFBX(fbxName));
}

void ShowGameObject(yunutyEngine::GameObject* obj)
{
	if (ImGui::TreeNode(obj->getName().c_str()))
	{
		for (auto& child : obj->GetChildren())
		{
			ShowGameObject(child);
		}

		ImGui::TreePop();
	}
}

void ShowFBXList()
{
	for (auto& each : g_gameObjVec)
	{
		ShowGameObject(each);
	}
}