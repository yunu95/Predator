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
#include <filesystem>

bool g_fbxLoad = false;
bool g_useIBL = true;
std::unordered_map<std::wstring, yunuGI::FBXData*> g_fbxMap;
yunuGI::FBXData* g_selectFBX = nullptr;
yunuGI::FBXData* g_prevFBX = nullptr;
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

void CreateMyWindow(HINSTANCE h_instance, HINSTANCE h_prev_instance, LPWSTR lp_cmd_line, int n_cmd_show);
void CreateToolWindow(HINSTANCE h_instance, HINSTANCE h_prev_instance, LPWSTR lp_cmd_line, int n_cmd_show);

WNDCLASS wc;
WNDCLASSEX wcEditor;

int monitorX;
int monitorY;

void ResizeBuffers();

std::string ConvertWideStringToUTF8(const std::wstring& wideString) {
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, wideString.c_str(), -1, nullptr, 0, nullptr, nullptr);
	std::string utf8String(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, wideString.c_str(), -1, &utf8String[0], size_needed, nullptr, nullptr);
	return utf8String;
}

void SaveAll(yunuGI::FBXData* data);
void ApplyAllMaterial(yunuGI::FBXData* data, yunuGI::IMaterial* material);


void FBXLoad();
void ShowFBXData(yunuGI::FBXData* data, bool isSave);
void ShowFBXList();
void ShowSeleteFBXInfo();
void CreateComboByTexture(std::string comboName, std::wstring& textureName, std::vector<yunuGI::ITexture*>& textureList);
void CreateComboByShader(std::string comboName, std::wstring& shaderName, std::vector<yunuGI::IShader*>& shaderList);

void ApplyMaterial(yunuGI::MaterialData& data, yunuGI::IMaterial* material);
void ApplyFirstMaterial(yunuGI::MaterialData& data, yunuGI::IMaterial* material);


void LoadResourcesRecursively();



void SaveFBXMaterial();
void LoadFBXMaterial();

void ImGuiUpdate();
void DrawMenuBar();

void InputUpdate();

std::filesystem::path SaveFileDialog(const char* filter = ".\0*.*\0", const char* initialDir = "");
std::filesystem::path LoadFileDialog(const char* filter = "All\0*.*\0", const char* initialDir = "");
std::vector<std::filesystem::path> GetSubdirectories(const std::filesystem::path& directoryPath = "");

std::filesystem::path currentPath = "";

bool isRunning = true;

std::mutex loopTodoRegistrationMutex;
// AddMainLoopTodo로 등록된 휘발성 콜백 함수들입니다.
// 매 루프가 종료될 때 이 컨테이너에 실행 동작들이 담겨있다면 모두 실행하고 내용을 비웁니다.
// 이 목록에 담긴 함수들이 실행되는 동안 게임 엔진 스레드는 동작을 정지합니다.
std::vector<std::function<void()>> loopRegistrations;

int APIENTRY wWinMain(_In_ HINSTANCE h_instance, _In_opt_ HINSTANCE h_prev_instance, _In_ LPWSTR lp_cmd_line, _In_ int n_cmd_show)
{
	monitorX = GetSystemMetrics(SM_CXSCREEN);
	monitorY = GetSystemMetrics(SM_CYSCREEN);

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

	/// Finalize
	yunutyEngine::graphics::Renderer::SingleInstance().Finalize();
	::DestroyWindow(g_hwnd);
	::UnregisterClass(wc.lpszClassName, wc.hInstance);
	::DestroyWindow(g_Toolhwnd);
	::UnregisterClass(wcEditor.lpszClassName, wcEditor.hInstance);

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

void CreateMyWindow(HINSTANCE h_instance, HINSTANCE h_prev_instance, LPWSTR lp_cmd_line, int n_cmd_show)
{
	// 윈도우 클래스 등록
	wc = {};
	wc.lpfnWndProc = WndProc;
	wc.hInstance = h_instance;
	wc.lpszClassName = L"MyWindowClass";
	RegisterClass(&wc);

	// 윈도우 생성
	g_hwnd = CreateWindow(
		L"MyWindowClass",   // 등록한 윈도우 클래스 이름
		L"My Window",       // 윈도우 제목
		WS_OVERLAPPEDWINDOW, // 윈도우 스타일
		(monitorX - 1920) / 2,    // x 좌표
		(monitorY - 1080) / 2,    // y 좌표
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

void CreateToolWindow(HINSTANCE h_instance, HINSTANCE h_prev_instance, LPWSTR lp_cmd_line, int n_cmd_show)
{
	// 윈도우 클래스 등록
	wcEditor = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProcTool, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ResourceTool"), NULL };
	RegisterClassEx(&wcEditor);

	// 윈도우 생성
	g_Toolhwnd = ::CreateWindow(wcEditor.lpszClassName, wcEditor.lpszClassName, WS_OVERLAPPEDWINDOW, monitorX - (1920 / 2), (monitorY - 1080) / 2 + 100, 1920, 1080, g_hwnd, NULL, wcEditor.hInstance, NULL);
}

void FBXLoad()
{
	if (g_selectFBX)
	{
		if (g_selectGameObject)
		{
			Scene::getCurrentScene()->DestroyGameObject(g_selectGameObject);
		}
	}
	g_selectFBX = nullptr;
	g_selectGameObject = nullptr;

	//LoadFBXMaterial();

	g_fbxLoad = true;

	// 재귀적으로 모든 FBX 로드하기
	const yunuGI::IResourceManager* resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();

	LoadResourcesRecursively();

	g_fbxMap = resourceManager->GetFBXDataMap();

	g_fbxLoad = false;
}

void ShowFBXData(yunuGI::FBXData* data, bool isSave)
{
	if (data->nodeName != L"RootNode")
	{
		if (data->materialVec.size() == 0)
		{
			return;
		}

		// nodeName이 "RootNode"가 아닌 경우에만 트리 노드를 생성
		if (ImGui::TreeNode(std::string{ data->nodeName.begin(), data->nodeName.end() }.c_str()))
		{
			if (isSave == false)
			{
				if (ImGui::IsItemClicked(0))
				{
					// 클릭된 노드의 정보를 변수에 저장
					g_selectFBX = data;
				}
			}
			else
			{
				g_selectFBX = data;
			}

			for (auto& child : data->child)
			{
				// 자식이 있을 때만 자식 노드를 보여줌
				ShowFBXData(child, isSave);
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
			ShowFBXData(child, isSave);
		}
	}
}

void ShowFBXList()
{
	for (auto& each : g_fbxMap)
	{
		ShowFBXData(each.second, false);
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

		std::string str = std::string{ g_selectFBX->meshName.begin(), g_selectFBX->meshName.end() };
		g_selectGameObject = Scene::getCurrentScene()->AddGameObjectFromFBX(str);

		ImGui::Text("DiffuseExposure :");
		ImGui::DragFloat("##DiffuseExposure", &g_selectFBX->diffuseExposure, 0.1f, 0.0, 10.0);

		//ImGui::InputFloat("DiffuseExposure", &g_selectFBX->diffuseExposure);
		ImGui::Text("AmbientExposure :");
		ImGui::DragFloat("##AmbientExposure", &g_selectFBX->ambientExposure, 0.1f, 0.0, 10.0);

		int materialIndex = 0;
		for (auto& each : g_selectFBX->materialVec)
		{
			if (g_selectFBX != g_prevFBX)
			{
				// 맨 처음 한 번은 머터리얼 값 적용
				ApplyFirstMaterial(each, resourceManager->GetMaterial(g_selectFBX->materialVec[materialIndex].materialName));
				
			}


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

			if (g_selectFBX->hasAnimation)
			{
				auto renderer = g_selectGameObject->GetChildren()[0]->GetComponent<yunutyEngine::graphics::SkinnedMesh>();
				ApplyMaterial(each, resourceManager->GetMaterial(g_selectFBX->materialVec[materialIndex].materialName));
			}
			else
			{
				auto renderer = g_selectGameObject->GetChildren()[0]->GetComponent<yunutyEngine::graphics::StaticMeshRenderer>();
				ApplyMaterial(each, resourceManager->GetMaterial(g_selectFBX->materialVec[materialIndex].materialName));
			}

			for (auto& each : g_fbxMap)
			{
				ApplyAllMaterial(each.second, resourceManager->GetMaterial(g_selectFBX->materialVec[materialIndex].materialName));
			}

			materialIndex++;
		}

		resourceManager->GetMesh(g_selectFBX->meshName)->SetDiffuseExposure(g_selectFBX->diffuseExposure);
		resourceManager->GetMesh(g_selectFBX->meshName)->SetAmbientExposure(g_selectFBX->ambientExposure);

		g_prevFBX = g_selectFBX;
	}
}

void ApplyAllMaterial(yunuGI::FBXData* data, yunuGI::IMaterial* material)
{
	const yunuGI::IResourceManager* resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
	if (data->nodeName != L"RootNode")
	{
		if (data->materialVec.size() != 0)
		{
			for (auto& each : data->materialVec)
			{
				if (each.materialName == material->GetName())
				{
					each.vs = material->GetVertexShader()->GetName();
					each.ps = material->GetPixelShader()->GetName();

					material->SetVertexShader(resourceManager->GetShader(each.vs));
					material->SetPixelShader(resourceManager->GetShader(each.ps));

					if (!each.albedoMap.empty())
					{
						auto texture = material->GetTexture(yunuGI::Texture_Type::ALBEDO);
						if (texture)
						{
							if (texture->GetName() != each.albedoMap)
							{
								each.albedoMap = texture->GetName();
							}

							material->SetTexture(yunuGI::Texture_Type::ALBEDO, texture);
						}
					}
					if (!each.normalMap.empty())
					{
						auto texture = material->GetTexture(yunuGI::Texture_Type::NORMAL);
						if (texture)
						{
							if (texture->GetName() != each.normalMap)
							{
								each.normalMap = texture->GetName();
							}

							material->SetTexture(yunuGI::Texture_Type::NORMAL, texture);
						}
					}
					if (!each.armMap.empty())
					{
						auto texture = material->GetTexture(yunuGI::Texture_Type::ARM);
						if (texture)
						{
							if (texture->GetName() != each.armMap)
							{
								each.armMap = texture->GetName();
							}

							material->SetTexture(yunuGI::Texture_Type::ARM, texture);
						}
					}
					if (!each.emissionMap.empty())
					{
						auto texture = material->GetTexture(yunuGI::Texture_Type::EMISSION);
						if (texture)
						{
							if (texture->GetName() != each.emissionMap)
							{
								each.emissionMap = texture->GetName();
							}

							material->SetTexture(yunuGI::Texture_Type::EMISSION, texture);
						}
					}
					if (!each.heightMap.empty())
					{
						auto texture = material->GetTexture(yunuGI::Texture_Type::HEIGHT);
						if (texture)
						{
							if (texture->GetName() != each.heightMap)
							{
								each.heightMap = texture->GetName();
							}

							material->SetTexture(yunuGI::Texture_Type::HEIGHT, texture);
						}
					}
					if (!each.opacityMap.empty())
					{
						auto texture = material->GetTexture(yunuGI::Texture_Type::OPACITY);
						if (texture)
						{
							if (texture->GetName() != each.opacityMap)
							{
								each.opacityMap = texture->GetName();
							}

							material->SetTexture(yunuGI::Texture_Type::OPACITY, texture);
						}
					}
					if (!each.temp0Map.empty())
					{
						auto texture = material->GetTexture(yunuGI::Texture_Type::Temp0);
						if (texture)
						{
							if (texture->GetName() != each.temp0Map)
							{
								each.temp0Map = texture->GetName();
							}

							material->SetTexture(yunuGI::Texture_Type::Temp0, texture);
						}
					}
					if (!each.temp1Map.empty())
					{
						auto texture = material->GetTexture(yunuGI::Texture_Type::Temp1);
						if (texture)
						{
							if (texture->GetName() != each.temp1Map)
							{
								each.temp1Map = texture->GetName();
							}

							material->SetTexture(yunuGI::Texture_Type::Temp1, texture);
						}
					}
					if (!each.temp2Map.empty())
					{
						auto texture = material->GetTexture(yunuGI::Texture_Type::Temp2);
						if (texture)
						{
							if (texture->GetName() != each.temp2Map)
							{
								each.temp2Map = texture->GetName();
							}

							material->SetTexture(yunuGI::Texture_Type::Temp2, texture);
						}
					}
					if (!each.temp3Map.empty())
					{
						auto texture = material->GetTexture(yunuGI::Texture_Type::Temp3);
						if (texture)
						{
							if (texture->GetName() != each.temp3Map)
							{
								each.temp3Map = texture->GetName();
							}

							material->SetTexture(yunuGI::Texture_Type::Temp3, texture);
						}
					}
					if (!each.temp4Map.empty())
					{
						auto texture = material->GetTexture(yunuGI::Texture_Type::Temp4);
						if (texture)
						{
							if (texture->GetName() != each.temp4Map)
							{
								each.temp4Map = texture->GetName();
							}

							material->SetTexture(yunuGI::Texture_Type::Temp4, texture);
						}
					}
					if (!each.temp5Map.empty())
					{
						auto texture = material->GetTexture(yunuGI::Texture_Type::Temp5);
						if (texture)
						{
							if (texture->GetName() != each.temp5Map)
							{
								each.temp5Map = texture->GetName();
							}

							material->SetTexture(yunuGI::Texture_Type::Temp5, texture);
						}
					}
					if (!each.temp6Map.empty())
					{
						auto texture = material->GetTexture(yunuGI::Texture_Type::Temp6);
						if (texture)
						{
							if (texture->GetName() != each.temp6Map)
							{
								each.temp6Map = texture->GetName();
							}

							material->SetTexture(yunuGI::Texture_Type::Temp6, texture);
						}
					}
					if (!each.temp7Map.empty())
					{
						auto texture = material->GetTexture(yunuGI::Texture_Type::Temp7);
						if (texture)
						{
							if (texture->GetName() != each.temp7Map)
							{
								each.temp7Map = texture->GetName();
							}

							material->SetTexture(yunuGI::Texture_Type::Temp7, texture);
						}
					}
					if (!each.temp8Map.empty())
					{
						auto texture = material->GetTexture(yunuGI::Texture_Type::Temp8);
						if (texture)
						{
							if (texture->GetName() != each.temp8Map)
							{
								each.temp8Map = texture->GetName();
							}

							material->SetTexture(yunuGI::Texture_Type::Temp8, texture);
						}
					}
					if (!each.temp9Map.empty())
					{
						auto texture = material->GetTexture(yunuGI::Texture_Type::Temp9);
						if (texture)
						{
							if (texture->GetName() != each.temp9Map)
							{
								each.temp9Map = texture->GetName();
							}

							material->SetTexture(yunuGI::Texture_Type::Temp9, texture);
						}
					}
					if (!each.temp10Map.empty())
					{
						auto texture = material->GetTexture(yunuGI::Texture_Type::Temp10);
						if (texture)
						{
							if (texture->GetName() != each.temp10Map)
							{
								each.temp10Map = texture->GetName();
							}

							material->SetTexture(yunuGI::Texture_Type::Temp10, texture);
						}
					}
					if (!each.temp11Map.empty())
					{
						auto texture = material->GetTexture(yunuGI::Texture_Type::Temp11);
						if (texture)
						{
							if (texture->GetName() != each.temp11Map)
							{
								each.temp11Map = texture->GetName();
							}

							material->SetTexture(yunuGI::Texture_Type::Temp11, texture);
						}
					}
				}
			}
		}
	}
	else
	{
		for (auto& each : data->child)
		{
			{
				ApplyAllMaterial(each, material);
			}
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

	ImGui::SameLine();
	ImGui::PushID(comboName.c_str());
	if (ImGui::Button("Reset"))
	{
		textureName = L"";
	}
	ImGui::PopID();
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


void ApplyMaterial(yunuGI::MaterialData& data, yunuGI::IMaterial* material)
{
	const yunuGI::IResourceManager* resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();

	material->SetVertexShader(resourceManager->GetShader(data.vs));
	material->SetPixelShader(resourceManager->GetShader(data.ps));

	if (!data.albedoMap.empty())
	{
		material->SetTexture(yunuGI::Texture_Type::ALBEDO, resourceManager->GetTexture(data.albedoMap));
	}
	if (!data.normalMap.empty())
	{
		material->SetTexture(yunuGI::Texture_Type::NORMAL, resourceManager->GetTexture(data.normalMap));
	}
	if (!data.armMap.empty())
	{
		material->SetTexture(yunuGI::Texture_Type::ARM, resourceManager->GetTexture(data.armMap));
	}
	if (!data.emissionMap.empty())
	{
		material->SetTexture(yunuGI::Texture_Type::EMISSION, resourceManager->GetTexture(data.emissionMap));
	}
	if (!data.heightMap.empty())
	{
		material->SetTexture(yunuGI::Texture_Type::HEIGHT, resourceManager->GetTexture(data.heightMap));
	}
	if (!data.opacityMap.empty())
	{
		material->SetTexture(yunuGI::Texture_Type::OPACITY, resourceManager->GetTexture(data.opacityMap));
	}
	if (!data.temp0Map.empty())
	{
		material->SetTexture(yunuGI::Texture_Type::Temp0, resourceManager->GetTexture(data.temp0Map));
	}
	if (!data.temp1Map.empty())
	{
		material->SetTexture(yunuGI::Texture_Type::Temp1, resourceManager->GetTexture(data.temp1Map));
	}
	if (!data.temp2Map.empty())
	{
		material->SetTexture(yunuGI::Texture_Type::Temp2, resourceManager->GetTexture(data.temp2Map));
	}
	if (!data.temp3Map.empty())
	{
		material->SetTexture(yunuGI::Texture_Type::Temp3, resourceManager->GetTexture(data.temp3Map));
	}
	if (!data.temp4Map.empty())
	{
		material->SetTexture(yunuGI::Texture_Type::Temp4, resourceManager->GetTexture(data.temp4Map));
	}
	if (!data.temp5Map.empty())
	{
		material->SetTexture(yunuGI::Texture_Type::Temp5, resourceManager->GetTexture(data.temp5Map));
	}
	if (!data.temp6Map.empty())
	{
		material->SetTexture(yunuGI::Texture_Type::Temp6, resourceManager->GetTexture(data.temp6Map));
	}
	if (!data.temp7Map.empty())
	{
		material->SetTexture(yunuGI::Texture_Type::Temp7, resourceManager->GetTexture(data.temp7Map));
	}
	if (!data.temp8Map.empty())
	{
		material->SetTexture(yunuGI::Texture_Type::Temp8, resourceManager->GetTexture(data.temp8Map));
	}
	if (!data.temp9Map.empty())
	{
		material->SetTexture(yunuGI::Texture_Type::Temp9, resourceManager->GetTexture(data.temp9Map));
	}
	if (!data.temp10Map.empty())
	{
		material->SetTexture(yunuGI::Texture_Type::Temp10, resourceManager->GetTexture(data.temp10Map));
	}
	if (!data.temp11Map.empty())
	{
		material->SetTexture(yunuGI::Texture_Type::Temp11, resourceManager->GetTexture(data.temp11Map));
	}
}



void ApplyFirstMaterial(yunuGI::MaterialData& data, yunuGI::IMaterial* material)
{
	const yunuGI::IResourceManager* resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();

	data.vs = material->GetVertexShader()->GetName();
	data.ps = material->GetPixelShader()->GetName();

	material->SetVertexShader(resourceManager->GetShader(data.vs));
	material->SetPixelShader(resourceManager->GetShader(data.ps));

	if (!data.albedoMap.empty())
	{
		auto texture = material->GetTexture(yunuGI::Texture_Type::ALBEDO);
		if (texture)
		{
			if (texture->GetName() != data.albedoMap)
			{
				data.albedoMap = texture->GetName();
			}

			material->SetTexture(yunuGI::Texture_Type::ALBEDO, texture);
		}
	}
	if (!data.normalMap.empty())
	{
		auto texture = material->GetTexture(yunuGI::Texture_Type::NORMAL);
		if (texture)
		{
			if (texture->GetName() != data.normalMap)
			{
				data.normalMap = texture->GetName();
			}

			material->SetTexture(yunuGI::Texture_Type::NORMAL, texture);
		}
	}
	if (!data.armMap.empty())
	{
		auto texture = material->GetTexture(yunuGI::Texture_Type::ARM);
		if (texture)
		{
			if (texture->GetName() != data.armMap)
			{
				data.armMap = texture->GetName();
			}

			material->SetTexture(yunuGI::Texture_Type::ARM, texture);
		}
	}
	if (!data.emissionMap.empty())
	{
		auto texture = material->GetTexture(yunuGI::Texture_Type::EMISSION);
		if (texture)
		{
			if (texture->GetName() != data.emissionMap)
			{
				data.emissionMap = texture->GetName();
			}

			material->SetTexture(yunuGI::Texture_Type::EMISSION, texture);
		}
	}
	if (!data.heightMap.empty())
	{
		auto texture = material->GetTexture(yunuGI::Texture_Type::HEIGHT);
		if (texture)
		{
			if (texture->GetName() != data.heightMap)
			{
				data.heightMap = texture->GetName();
			}

			material->SetTexture(yunuGI::Texture_Type::HEIGHT, texture);
		}
	}
	if (!data.opacityMap.empty())
	{
		auto texture = material->GetTexture(yunuGI::Texture_Type::OPACITY);
		if (texture)
		{
			if (texture->GetName() != data.opacityMap)
			{
				data.opacityMap = texture->GetName();
			}

			material->SetTexture(yunuGI::Texture_Type::OPACITY, texture);
		}
	}
	if (!data.temp0Map.empty())
	{
		auto texture = material->GetTexture(yunuGI::Texture_Type::Temp0);
		if (texture)
		{
			if (texture->GetName() != data.temp0Map)
			{
				data.temp0Map = texture->GetName();
			}

			material->SetTexture(yunuGI::Texture_Type::Temp0, texture);
		}
	}
	if (!data.temp1Map.empty())
	{
		auto texture = material->GetTexture(yunuGI::Texture_Type::Temp1);
		if (texture)
		{
			if (texture->GetName() != data.temp1Map)
			{
				data.temp1Map = texture->GetName();
			}

			material->SetTexture(yunuGI::Texture_Type::Temp1, texture);
		}
	}
	if (!data.temp2Map.empty())
	{
		auto texture = material->GetTexture(yunuGI::Texture_Type::Temp2);
		if (texture)
		{
			if (texture->GetName() != data.temp2Map)
			{
				data.temp2Map = texture->GetName();
			}

			material->SetTexture(yunuGI::Texture_Type::Temp2, texture);
		}
	}
	if (!data.temp3Map.empty())
	{
		auto texture = material->GetTexture(yunuGI::Texture_Type::Temp3);
		if (texture)
		{
			if (texture->GetName() != data.temp3Map)
			{
				data.temp3Map = texture->GetName();
			}

			material->SetTexture(yunuGI::Texture_Type::Temp3, texture);
		}
	}
	if (!data.temp4Map.empty())
	{
		auto texture = material->GetTexture(yunuGI::Texture_Type::Temp4);
		if (texture)
		{
			if (texture->GetName() != data.temp4Map)
			{
				data.temp4Map = texture->GetName();
			}

			material->SetTexture(yunuGI::Texture_Type::Temp4, texture);
		}
	}
	if (!data.temp5Map.empty())
	{
		auto texture = material->GetTexture(yunuGI::Texture_Type::Temp5);
		if (texture)
		{
			if (texture->GetName() != data.temp5Map)
			{
				data.temp5Map = texture->GetName();
			}

			material->SetTexture(yunuGI::Texture_Type::Temp5, texture);
		}
	}
	if (!data.temp6Map.empty())
	{
		auto texture = material->GetTexture(yunuGI::Texture_Type::Temp6);
		if (texture)
		{
			if (texture->GetName() != data.temp6Map)
			{
				data.temp6Map = texture->GetName();
			}

			material->SetTexture(yunuGI::Texture_Type::Temp6, texture);
		}
	}
	if (!data.temp7Map.empty())
	{
		auto texture = material->GetTexture(yunuGI::Texture_Type::Temp7);
		if (texture)
		{
			if (texture->GetName() != data.temp7Map)
			{
				data.temp7Map = texture->GetName();
			}

			material->SetTexture(yunuGI::Texture_Type::Temp7, texture);
		}
	}
	if (!data.temp8Map.empty())
	{
		auto texture = material->GetTexture(yunuGI::Texture_Type::Temp8);
		if (texture)
		{
			if (texture->GetName() != data.temp8Map)
			{
				data.temp8Map = texture->GetName();
			}

			material->SetTexture(yunuGI::Texture_Type::Temp8, texture);
		}
	}
	if (!data.temp9Map.empty())
	{
		auto texture = material->GetTexture(yunuGI::Texture_Type::Temp9);
		if (texture)
		{
			if (texture->GetName() != data.temp9Map)
			{
				data.temp9Map = texture->GetName();
			}

			material->SetTexture(yunuGI::Texture_Type::Temp9, texture);
		}
	}
	if (!data.temp10Map.empty())
	{
		auto texture = material->GetTexture(yunuGI::Texture_Type::Temp10);
		if (texture)
		{
			if (texture->GetName() != data.temp10Map)
			{
				data.temp10Map = texture->GetName();
			}

			material->SetTexture(yunuGI::Texture_Type::Temp10, texture);
		}
	}
	if (!data.temp11Map.empty())
	{
		auto texture = material->GetTexture(yunuGI::Texture_Type::Temp11);
		if (texture)
		{
			if (texture->GetName() != data.temp11Map)
			{
				data.temp11Map = texture->GetName();
			}

			material->SetTexture(yunuGI::Texture_Type::Temp11, texture);
		}
	}
}

void SaveFBXMaterial()
{
	std::filesystem::path path = SaveFileDialog("Resource File (*.scres)\0*.scres\0");
	const yunuGI::IResourceManager* resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();

	//auto dataMap = resourceManager->GetFBXDataMap();
	//for (auto& each : dataMap)
	//{
	//	SaveAll(each.second);
	//}


	resourceManager->SaveFBXData(path);
}

void LoadFBXMaterial()
{
	std::filesystem::path path = LoadFileDialog("Resource File (*.scres)\0*.scres\0");

	const yunuGI::IResourceManager* resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
	resourceManager->LoadFBXData(path);

	currentPath = path;
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

		InputUpdate();

		ImGui::Begin("ResourceToolDockSpace", nullptr, window_flags);

		// Dockspace
		ImGui::DockSpace(ImGui::GetID("ResourceToolDockspace"));

		DrawMenuBar();

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

void DrawMenuBar()
{
	ImGui::BeginMenuBar();

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2());
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4());
	if (ImGui::Button("Load"))
	{
		FBXLoad();
	}
	if (ImGui::Button("Save As"))
	{
		SaveFBXMaterial();
	}
	if (ImGui::Button("Save"))
	{
		if (!currentPath.empty())
		{
			const yunuGI::IResourceManager* resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
			resourceManager->SaveFBXData(currentPath);
		}
	}
	ImGui::Text(" | Use IBL : ");
	if (ImGui::Checkbox("##Use IBL", &g_useIBL))
	{
		yunutyEngine::graphics::Renderer::SingleInstance().SetUseIBL(g_useIBL);
	}

	if (!currentPath.empty())
	{
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(currentPath.string().c_str()).x - 10);
		ImGui::Text(currentPath.string().c_str());
	}

	ImGui::PopStyleVar();
	ImGui::PopStyleColor();
	ImGui::EndMenuBar();
}

std::filesystem::path SaveFileDialog(const char* filter, const char* initialDir)
{
	OPENFILENAMEA ofn;       // common dialog box structure
	CHAR szFile[260] = { 0 };       // if using TCHAR macros

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = g_Toolhwnd;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	if (initialDir != "")
	{
		ofn.lpstrInitialDir = initialDir;
	}

	if (GetSaveFileNameA(&ofn) == TRUE)
	{
		std::string fp = ofn.lpstrFile;
		std::replace(fp.begin(), fp.end(), '\\', '/');
		return std::filesystem::path(fp);
	}

	return std::filesystem::path();
}

std::filesystem::path LoadFileDialog(const char* filter, const char* initialDir)
{
	OPENFILENAMEA ofn;       // common dialog box structure
	CHAR szFile[260] = { 0 };       // if using TCHAR macros

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = g_Toolhwnd;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	if (initialDir != "")
	{
		ofn.lpstrInitialDir = initialDir;
	}

	if (GetOpenFileNameA(&ofn) == TRUE)
	{
		std::string fp = ofn.lpstrFile;
		std::replace(fp.begin(), fp.end(), '\\', '/');
		return std::filesystem::path(fp);
	}

	return std::filesystem::path();
}

std::vector<std::filesystem::path> GetSubdirectories(const std::filesystem::path& directoryPath)
{
	std::vector<std::filesystem::path> subdirectories;
	WIN32_FIND_DATAA findData;
	HANDLE hFind = FindFirstFileA((directoryPath.string() + "\\*").c_str(), &findData);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (strcmp(findData.cFileName, ".") != 0 && strcmp(findData.cFileName, "..") != 0)
				{
					subdirectories.push_back(findData.cFileName);
				}
			}
		} while (FindNextFileA(hFind, &findData));
		FindClose(hFind);
	}

	return subdirectories;
}

void InputUpdate()
{
	if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
	{
		if (ImGui::IsKeyPressed(ImGuiKey_S, false))
		{
			if (!currentPath.empty())
			{
				const yunuGI::IResourceManager* resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
				resourceManager->SaveFBXData(currentPath);
			}
			else
			{
				SaveFBXMaterial();
			}
		}
	}
}


void LoadResourcesRecursively()
{
	{
		const yunuGI::IResourceManager* resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();

		/// SCRES 우선 로드
		resourceManager->LoadFile("FBXMaterial.scres");

		// 나머지 기타등등 파일들 로드하기
		{
			namespace fs = std::filesystem;
			std::set<std::string> validExtensions{ ".jpg", ".bmp", ".tga", ".dds", ".cso" ,".png" };
			fs::path basePath{ "./" };
			try
			{
				if (fs::exists(basePath) && fs::is_directory(basePath))
				{
					for (const auto& entry : fs::recursive_directory_iterator(basePath))
					{
						if (fs::is_regular_file(entry) && validExtensions.contains(entry.path().extension().string()))
						{
							auto relativePath = fs::relative(entry.path(), basePath);
							resourceManager->LoadFile(relativePath.string().c_str());
						}
					}
				}
			}
			catch (const fs::filesystem_error& err) {
				std::cerr << "Error: " << err.what() << std::endl;
			}
		}

		// FBX 로드하기
		{
			auto directorList = GetSubdirectories("FBX");
			for (auto each : directorList)
			{
				resourceManager->LoadFile(("FBX/" + each.string()).c_str());
			}
		}
	}


}


void SaveAll(yunuGI::FBXData* data)
{
	if (data->nodeName != L"RootNode")
	{
		if (data->materialVec.size() == 0)
		{
			return;
		}

		if (g_selectFBX == data) return;

		g_selectFBX = data;
		// 다중메쉬가 아닐 때만 허용됨.
		ShowSeleteFBXInfo();
	}

	for (auto& each : data->child)
	{
		SaveAll(each);
	}
}