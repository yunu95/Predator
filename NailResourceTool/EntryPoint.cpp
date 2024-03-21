#include "YunutyEngine.h"

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "ImGuizmo/ImGuizmo.h"
#include "TestUtilGraphicsTestCam.h"
#include <d3d11.h>
#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <dxgi1_4.h>
#include <unordered_map>
#include <locale>
#include <codecvt>

bool g_fbxLoad = false;
std::unordered_map<std::wstring, yunuGI::FBXData*> g_fbxMap;
yunuGI::FBXData* g_selectFBX = nullptr;
yunutyEngine::GameObject* g_selectGameObject = nullptr;
// Data
static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain1* g_pSwapChain = nullptr;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

HWND g_hwnd = nullptr;
HWND g_Toolhwnd = nullptr;

ImVec2 dockspaceArea = ImVec2();
ImVec2 dockspaceStartPoint = ImVec2();

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

void ResizeBuffers();

std::string ConvertWideStringToUTF8(const std::wstring& wideString) {
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, wideString.c_str(), -1, nullptr, 0, nullptr, nullptr);
	std::string utf8String(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, wideString.c_str(), -1, &utf8String[0], size_needed, nullptr, nullptr);
	return utf8String;
}

void FBXLoad();
void ShowFBXData(yunuGI::FBXData* data);
void ShowFBXList();
void ShowSeleteFBXInfo();
void CreateComboByTexture(std::string comboName, std::wstring& textureName, std::vector<yunuGI::ITexture*>& textureList);
void CreateComboByShader(std::string comboName, std::wstring& shaderName, std::vector<yunuGI::IShader*>& shaderList);

void SaveFBXMaterial();
void LoadFBXMaterial();

void ImGuiUpdate();

bool isRunning = true;

std::mutex loopTodoRegistrationMutex;
// AddMainLoopTodo로 등록된 휘발성 콜백 함수들입니다.
// 매 루프가 종료될 때 이 컨테이너에 실행 동작들이 담겨있다면 모두 실행하고 내용을 비웁니다.
// 이 목록에 담긴 함수들이 실행되는 동안 게임 엔진 스레드는 동작을 정지합니다.
std::vector<std::function<void()>> loopRegistrations;

int WINAPI main(HINSTANCE h_instance, HINSTANCE h_prev_instance, LPSTR lp_cmd_line, int n_cmd_show)
{
	CreateMyWindow(h_instance, h_prev_instance, lp_cmd_line, n_cmd_show);

	yunutyEngine::YunutyCycle::SingleInstance().preThreadAction = [&]()
	{
		CreateToolWindow(h_instance, nullptr, lp_cmd_line, n_cmd_show);

		// Setup Platform/Renderer backends
		g_pd3dDevice = reinterpret_cast<ID3D11Device*>(yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager()->GetDevice());
		g_pd3dDeviceContext = reinterpret_cast<ID3D11DeviceContext*>(yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager()->GetDeviceContext());

		// Initialize Direct3D
		if (!CreateDeviceD3D(g_Toolhwnd))
		{
			CleanupDeviceD3D();
			return 1;
		}

		::ShowWindow(g_Toolhwnd, SW_SHOWDEFAULT);
		::UpdateWindow(g_Toolhwnd);

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
		//io.ConfigViewportsNoAutoMerge = true;
		//io.ConfigViewportsNoTaskBarIcon = true;
		//io.ConfigViewportsNoDefaultParent = true;
		//io.ConfigDockingAlwaysTabBar = true;
		//io.ConfigDockingTransparentPayload = true;
		//io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI: Experimental. THIS CURRENTLY DOESN'T WORK AS EXPECTED. DON'T USE IN USER APP!
		//io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; // FIXME-DPI: Experimental.

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

		// Load Fonts
		// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
		// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
		// - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
		// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
		// - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
		// - Read 'docs/FONTS.md' for more instructions and details.
		// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
		//io.Fonts->AddFontDefault();
		//io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
		//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
		//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
		//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
		//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
		//IM_ASSERT(font != nullptr);
	};

	yunutyEngine::YunutyCycle::SingleInstance().postUpdateAction = [&]() { ImGuiUpdate(); };
	yunutyEngine::YunutyCycle::SingleInstance().postThreadAction = []()
	{
	};

	yunutyEngine::graphics::Renderer::SingleInstance().LoadGraphicsDll(L"NailEngine.dll");
	yunutyEngine::graphics::Renderer::SingleInstance().SetResolution(1920, 1080);
	yunutyEngine::graphics::Renderer::SingleInstance().SetOutputWindow(g_hwnd);

	yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());

	yunutyEngine::Collider2D::SetIsOnXYPlane(false);
	auto directionalLight = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	directionalLight->GetTransform()->SetLocalRotation(Quaternion{ Vector3d{50,-30,0} });
	directionalLight->GetTransform()->SetLocalPosition(Vector3d{ 0,0,-20 });
	auto light = directionalLight->AddComponent<yunutyEngine::graphics::DirectionalLight>();
	auto color = yunuGI::Color{ 1,1,1,1.f };
	light->GetGI().SetLightDiffuseColor(color);

	auto camObj = yunutyEngine::Scene::getCurrentScene()->AddGameObject();
	auto cam = camObj->AddComponent<tests::GraphicsTestCam>();
	camObj->GetTransform()->SetLocalPosition(yunutyEngine::Vector3{ 0,0,-20 });
	cam->SetCameraMain();

	yunutyEngine::YunutyCycle::SingleInstance().Play();
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
		}

		// 게임 엔진을 멈추고 동작을 실행하는 부분
		{
			std::scoped_lock lock{ loopTodoRegistrationMutex };
			if (!loopRegistrations.empty())
			{
				//std::unique_lock preupdateLock{YunutyCycle::SingleInstance().preUpdateMutex};
				//std::unique_lock updateLock{YunutyCycle::SingleInstance().updateMutex};

				for (auto each : loopRegistrations)
					each();
				loopRegistrations.clear();
			}
		}
		if (!YunutyCycle::SingleInstance().IsGameRunning())
		{
			YunutyCycle::SingleInstance().Release();
			isRunning = false;
		}
	}
	if (YunutyCycle::SingleInstance().IsGameRunning())
		YunutyCycle::SingleInstance().Release();


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

LRESULT CALLBACK WndProcTool(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) 
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, message, w_param, l_param))
		return true;

	switch (message) {
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_SIZE:
		{
			if (w_param == SIZE_MINIMIZED)
				return 0;

			RECT rect = RECT();
			GetClientRect(hwnd, &rect);
			dockspaceArea = ImVec2(rect.right - rect.left, rect.bottom - rect.top);

			POINT windowPos = POINT();
			ClientToScreen(hwnd, &windowPos);
			dockspaceStartPoint = ImVec2(windowPos.x, windowPos.y);

			ResizeBuffers();

			return 0;
		}
		case WM_MOVE:
		{
			POINT windowPos = POINT();
			ClientToScreen(hwnd, &windowPos);
			dockspaceStartPoint = ImVec2(windowPos.x, windowPos.y);

			return 0;
		}
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

void ResizeBuffers()
{
	if (g_mainRenderTargetView && g_pSwapChain)
	{
		g_mainRenderTargetView->Release();
		g_pSwapChain->ResizeBuffers(0, dockspaceArea.x, dockspaceArea.y, DXGI_FORMAT_UNKNOWN, 0);
		CreateRenderTarget();
	}
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
}

void FBXLoad()
{
	g_fbxLoad = true;

	const yunuGI::IResourceManager* resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
	resourceManager->LoadFile("LeavesVS.cso");
	resourceManager->LoadFile("LeavesPS.cso");
	resourceManager->LoadFile("Stage_1_FloorPS.cso");

	resourceManager->LoadFile("Texture/VertexColor/T_Dirt_ARM.png");
	resourceManager->LoadFile("Texture/VertexColor/T_Dirt_BaseColor.png");
	resourceManager->LoadFile("Texture/VertexColor/T_Dirt_Normal.png");

	resourceManager->LoadFile("Texture/VertexColor/T_Grass_ARM.png");
	resourceManager->LoadFile("Texture/VertexColor/T_Grass_BaseColor.png");
	resourceManager->LoadFile("Texture/VertexColor/T_Grass_Normal.png");

	resourceManager->LoadFile("Texture/VertexColor/T_GrassBlend_ARM.png");
	resourceManager->LoadFile("Texture/VertexColor/T_GrassBlend_BaseColor.png");
	resourceManager->LoadFile("Texture/VertexColor/T_GrassBlend_Normal.png");

	resourceManager->LoadFile("Texture/VertexColor/T_Tile_ARM.png");
	resourceManager->LoadFile("Texture/VertexColor/T_Tile_BaseColor.png");
	resourceManager->LoadFile("Texture/VertexColor/T_Tile_Normal.png");

	resourceManager->LoadFile("Texture/VertexColor/T_TileBlend_ARM.png");
	resourceManager->LoadFile("Texture/VertexColor/T_TileBlend_BaseColor.png");
	resourceManager->LoadFile("Texture/VertexColor/T_TileBlend_Normal.png");

	resourceManager->LoadFile("FBX/SM_VertexColor");
	resourceManager->LoadFile("FBX/SKM_Monster1");
	resourceManager->LoadFile("FBX/SKM_Monster2");
	resourceManager->LoadFile("FBX/SKM_Robin");
	resourceManager->LoadFile("FBX/SM_Bush_001");
	resourceManager->LoadFile("FBX/SM_Bush_002");
	resourceManager->LoadFile("FBX/SM_Trunk_001");
	resourceManager->LoadFile("FBX/SM_CastleWall");
	resourceManager->LoadFile("FBX/SM_CastleWall_Door");
	resourceManager->LoadFile("FBX/SM_CastleWall_Pillar");
	resourceManager->LoadFile("FBX/SM_Chair");
	resourceManager->LoadFile("FBX/SM_CupTower");
	resourceManager->LoadFile("FBX/SM_Fork");
	resourceManager->LoadFile("FBX/SM_GuideBook");
	resourceManager->LoadFile("FBX/SM_Hat01");
	resourceManager->LoadFile("FBX/SM_Hat02");
	resourceManager->LoadFile("FBX/SM_SmallBush_001");
	resourceManager->LoadFile("FBX/SM_Stone_001");
	resourceManager->LoadFile("FBX/SM_Stone_002");
	resourceManager->LoadFile("FBX/SM_Stump");
	resourceManager->LoadFile("FBX/SM_Temple_Book_etc");
	resourceManager->LoadFile("FBX/SM_Temple_Books");
	resourceManager->LoadFile("FBX/SM_Temple_Floor");
	resourceManager->LoadFile("FBX/SM_Temple_Pillar");
	resourceManager->LoadFile("FBX/SM_Temple_Pillar_Broken");
	resourceManager->LoadFile("FBX/SM_Temple_Rabbit");
	resourceManager->LoadFile("FBX/SM_Mushroom01");
	resourceManager->LoadFile("FBX/SM_Mushroom02");
	resourceManager->LoadFile("FBX/SM_Temple_Welcome");
	resourceManager->LoadFile("FBX/SM_Stage1_Floor");

	g_fbxMap = resourceManager->GetFBXDataMap();

	g_fbxLoad = false;
}

void ShowFBXData(yunuGI::FBXData* data)
{
	if (data->nodeName != L"RootNode")
	{
		// nodeName이 "RootNode"가 아닌 경우에만 트리 노드를 생성
		if (ImGui::TreeNode(std::string{ data->nodeName.begin(), data->nodeName.end() }.c_str()))
		{
			if (ImGui::IsItemClicked(0))
			{
				// 클릭된 노드의 정보를 변수에 저장
				g_selectFBX = data;
			}

			for (auto& child : data->child)
			{
				// 자식이 있을 때만 자식 노드를 보여줌
				ShowFBXData(child);
			}

			ImGui::TreePop();
		}
	}
	else
	{
		// "RootNode"인 경우에 대한 처리를 원하면 이 부분에 추가
		for (auto& child : data->child)
		{
			// "RootNode"이 아닌 경우에만 자식 노드를 보여줌
			ShowFBXData(child);
		}
	}
}

void ShowFBXList()
{
	for (auto& each : g_fbxMap)
	{
		ShowFBXData(each.second);
	}
}

void ShowSeleteFBXInfo()
{
	const yunuGI::IResourceManager* resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
	std::vector<yunuGI::ITexture*>& textureList = resourceManager->GetTextureList();

	if (g_selectFBX)
	{
		if (g_selectGameObject)
		{
			Scene::getCurrentScene()->DestroyGameObject(g_selectGameObject);
		}
		std::string str = std::string{ g_selectFBX->meshName.begin(), g_selectFBX->meshName.end()};
		g_selectGameObject = Scene::getCurrentScene()->AddGameObjectFromFBX(str);


		for (auto& each : g_selectFBX->materialVec)
		{
			ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "MaterialName : %ls", each.materialName.c_str());

			/// Texture
			ImGui::Text("AlbedoMap : %ls", each.albedoMap.c_str());
			CreateComboByTexture(std::string{ "##" } + std::string{ each.materialName.begin(),each.materialName.end() } + std::string{ "AlbedoMap" },
				each.albedoMap, textureList);

			ImGui::Text("NormalMap : %ls", each.normalMap.c_str());
			CreateComboByTexture(std::string{ "##" } + std::string{ each.materialName.begin(),each.materialName.end() } + std::string{ "NormalMap" },
				each.normalMap, textureList);

			ImGui::Text("ARMMap : %ls", each.armMap.c_str());
			CreateComboByTexture(std::string{ "##" } + std::string{ each.materialName.begin(),each.materialName.end() } + std::string{ "ARMMap" },
				each.armMap, textureList);

			ImGui::Text("EmissionMap : %ls", each.emissionMap.c_str());
			CreateComboByTexture(std::string{ "##" } + std::string{ each.materialName.begin(),each.materialName.end() } + std::string{ "EmissionMap" },
				each.emissionMap, textureList);

			ImGui::Text("OpacityMap : %ls", each.opacityMap.c_str());
			CreateComboByTexture(std::string{ "##" } + std::string{ each.materialName.begin(),each.materialName.end() } + std::string{ "OpacityMap" },
				each.opacityMap, textureList);

			ImGui::Text("HeightMap : %ls", each.heightMap.c_str());
			CreateComboByTexture(std::string{ "##" } + std::string{ each.materialName.begin(),each.materialName.end() } + std::string{ "HeightMap" },
				each.heightMap, textureList);

			ImGui::Text("Temp0Map : %ls", each.temp0Map.c_str());
			CreateComboByTexture(std::string{ "##" } + std::string{ each.materialName.begin(),each.materialName.end() } + std::string{ "Temp0Map" },
				each.temp0Map, textureList);

			ImGui::Text("Temp1Map : %ls", each.temp1Map.c_str());
			CreateComboByTexture(std::string{ "##" } + std::string{ each.materialName.begin(),each.materialName.end() } + std::string{ "Temp1Map" },
				each.temp1Map, textureList);

			ImGui::Text("Temp2Map : %ls", each.temp2Map.c_str());
			CreateComboByTexture(std::string{ "##" } + std::string{ each.materialName.begin(),each.materialName.end() } + std::string{ "Temp2Map" },
				each.temp2Map, textureList);

			ImGui::Text("Temp3Map : %ls", each.temp3Map.c_str());
			CreateComboByTexture(std::string{ "##" } + std::string{ each.materialName.begin(),each.materialName.end() } + std::string{ "Temp3Map" },
				each.temp3Map, textureList);

			ImGui::Text("Temp4Map : %ls", each.temp4Map.c_str());
			CreateComboByTexture(std::string{ "##" } + std::string{ each.materialName.begin(),each.materialName.end() } + std::string{ "Temp4Map" },
				each.temp4Map, textureList);

			ImGui::Text("Temp5Map : %ls", each.temp5Map.c_str());
			CreateComboByTexture(std::string{ "##" } + std::string{ each.materialName.begin(),each.materialName.end() } + std::string{ "Temp5Map" },
				each.temp5Map, textureList);

			ImGui::Text("Temp6Map : %ls", each.temp6Map.c_str());
			CreateComboByTexture(std::string{ "##" } + std::string{ each.materialName.begin(),each.materialName.end() } + std::string{ "Temp6Map" },
				each.temp6Map, textureList);

			ImGui::Text("Temp7Map : %ls", each.temp7Map.c_str());
			CreateComboByTexture(std::string{ "##" } + std::string{ each.materialName.begin(),each.materialName.end() } + std::string{ "Temp7Map" },
				each.temp7Map, textureList);

			ImGui::Text("Temp8Map : %ls", each.temp8Map.c_str());
			CreateComboByTexture(std::string{ "##" } + std::string{ each.materialName.begin(),each.materialName.end() } + std::string{ "Temp8Map" },
				each.temp8Map, textureList);

			ImGui::Text("Temp9Map : %ls", each.temp9Map.c_str());
			CreateComboByTexture(std::string{ "##" } + std::string{ each.materialName.begin(),each.materialName.end() } + std::string{ "Temp9Map" },
				each.temp9Map, textureList);

			ImGui::Text("Temp10Map : %ls", each.temp10Map.c_str());
			CreateComboByTexture(std::string{ "##" } + std::string{ each.materialName.begin(),each.materialName.end() } + std::string{ "Temp10Map" },
				each.temp10Map, textureList);

			ImGui::Text("Temp11Map : %ls", each.temp11Map.c_str());
			CreateComboByTexture(std::string{ "##" } + std::string{ each.materialName.begin(),each.materialName.end() } + std::string{ "Temp11Map" },
				each.temp11Map, textureList);


			/// Shader
			std::vector<yunuGI::IShader*>& shaderList = resourceManager->GetShaderList();

			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "VertexShader : %ls", each.vs.c_str());
			CreateComboByShader(std::string{ "##" } + std::string{ each.materialName.begin(),each.materialName.end() } + std::string{ "VertexShader" },
				each.vs, shaderList);

			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "PixelShader : %ls", each.ps.c_str());
			CreateComboByShader(std::string{ "##" } + std::string{ each.materialName.begin(),each.materialName.end() } + std::string{ "PixelShader" },
				each.ps, shaderList);
		}
	}
}

void CreateComboByTexture(std::string comboName, std::wstring& textureName, std::vector<yunuGI::ITexture*>& textureList)
{
	if (ImGui::BeginCombo(comboName.c_str(), std::string{ textureName.begin(),textureName.end() }.c_str()))
	{
		for (auto& texture : textureList)
		{
			if (ImGui::Selectable(std::string{ texture->GetName().begin(),texture->GetName().end() }.c_str()))
			{
				textureName = texture->GetName();
			}
		}

		ImGui::EndCombo();
	}
}

void CreateComboByShader(std::string comboName, std::wstring& shaderName, std::vector<yunuGI::IShader*>& shaderList)
{
	if (ImGui::BeginCombo(comboName.c_str(), std::string{ shaderName.begin(),shaderName.end() }.c_str()))
	{
		for (auto& shader : shaderList)
		{
			if (ImGui::Selectable(std::string{ shader->GetName().begin(),shader->GetName().end() }.c_str()))
			{
				shaderName = shader->GetName();
			}
		}

		ImGui::EndCombo();
	}
}

void SaveFBXMaterial()
{
	const yunuGI::IResourceManager* resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
	resourceManager->SaveFBXData();
}

void LoadFBXMaterial()
{
	const yunuGI::IResourceManager* resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
	resourceManager->LoadFBXData();
}

void ImGuiUpdate()
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
	}

	if (!isRunning)
	{
		return;
	}

	//Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	window_flags |= ImGuiWindowFlags_MenuBar;
	window_flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	{
		ImGui::SetNextWindowSize(dockspaceArea);
		ImGui::SetNextWindowPos(dockspaceStartPoint);

		ImGui::Begin("DockSpace", nullptr, window_flags);

		// Dockspace
		ImGui::DockSpace(ImGui::GetID("MyDockspace"));

		{
			ImGui::Begin("Buttons");                          // Create a window called "Hello, world!" and append into it.

			// 바꾼 머터리얼을 저장하는 버튼
			if (ImGui::Button("SaveButton"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			{
				SaveFBXMaterial();
			}

			// 바뀐 머터리얼을 로드하는 버튼
			if (ImGui::Button("LoadButton"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			{
				LoadFBXMaterial();
			}

			// FBX를 로드하는 버튼
			if (ImGui::Button("FBXLoadButton"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			{
				FBXLoad();
			}

			ImGui::End();
		}

		{
			ImGui::Begin("FBXList");

			ShowFBXList();

			ImGui::End();
		}

		{
			ImGui::Begin("Info");

			ShowSeleteFBXInfo();

			ImGui::End();
		}

		{
			ImGui::Begin("RenderImage");

			auto size = ImGui::GetContentRegionAvail();

			// 그려지는 영역에 맞게 화면 비 재구성
			auto rect = yunutyEngine::graphics::Renderer::SingleInstance().GetResolution();
			float ratio = (float)rect.y / (float)rect.x;
			auto winMin = ImGui::GetWindowContentRegionMin();
			auto winMax = ImGui::GetWindowContentRegionMax();
			ImVec2 newRegion(winMax.x - winMin.x, winMax.y - winMin.y);
			float newRegionRatio = newRegion.y / newRegion.x;

			if (newRegionRatio >= ratio)
			{
				newRegion.y = newRegion.x * ratio;
			}
			else
			{
				newRegion.x = newRegion.y / ratio;
			}

			ImGui::Image(
				reinterpret_cast<ImTextureID>(yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager()->GetFinalRenderImage()),
				ImVec2(newRegion.x, newRegion.y)
			);
			ImGui::End();
		}

		ImGui::End();
	}

	ImGui::Render();
	
	const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };

	g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
	g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Update and Render additional Platform Windows
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	//g_EditorpSwapChain->Present(1, 0); // Present with vsync
	g_pSwapChain->Present(0, 0); // Present without vsync
}