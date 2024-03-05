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
#include <unordered_map>
#include <locale>
#include <codecvt>

bool g_fbxLoad = false;
std::unordered_map<std::wstring, yunuGI::FBXData*> g_fbxMap;
yunuGI::FBXData* g_selectFBX = nullptr;
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

		if (g_fbxLoad)
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



			ImGui::Begin("FBXList");

			ShowFBXList();

			ImGui::End();




			ImGui::Begin("Info");

			ShowSeleteFBXInfo();

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

void FBXLoad()
{
	g_fbxLoad = true;

	const yunuGI::IResourceManager* resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
	// Shader
	resourceManager->LoadFile("LeavesVS.cso");
	resourceManager->LoadFile("LeavesPS.cso");

	// Model
	resourceManager->LoadFile("FBX/Monster2");
	resourceManager->LoadFile("FBX/SM_Bush_001");
	resourceManager->LoadFile("FBX/SM_Bush_002");
	//resourceManager->LoadFile("FBX/SM_CastleWall");
	//resourceManager->LoadFile("FBX/SM_CastleWall_Door");
	//resourceManager->LoadFile("FBX/SM_CastleWall_Pillar");
	//resourceManager->LoadFile("FBX/SM_Chair");
	//resourceManager->LoadFile("FBX/SM_Cuptower");
	//resourceManager->LoadFile("FBX/SM_Fork");
	//resourceManager->LoadFile("FBX/SM_GuideBook");
	//resourceManager->LoadFile("FBX/SM_Hat01");
	//resourceManager->LoadFile("FBX/SM_Hat02");
	//resourceManager->LoadFile("FBX/SM_SmallBush_001");
	//resourceManager->LoadFile("FBX/SM_Stone_001");
	//resourceManager->LoadFile("FBX/SM_Stone_002");
	//resourceManager->LoadFile("FBX/SM_Stump");
	//resourceManager->LoadFile("FBX/SM_Temple_Book_etc");
	//resourceManager->LoadFile("FBX/SM_Temple_Books");
	//resourceManager->LoadFile("FBX/SM_Temple_Floor");
	//resourceManager->LoadFile("FBX/SM_Temple_Pillar");
	//resourceManager->LoadFile("FBX/SM_Temple_Pillar_Broken");
	//resourceManager->LoadFile("FBX/SM_Temple_Rabbit");
	//resourceManager->LoadFile("FBX/SM_Temple_Stairs");
	//resourceManager->LoadFile("FBX/SM_Temple_Welcome");
	//resourceManager->LoadFile("FBX/SM_Trunk_001");

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

			ImGui::Text("Temp3Map : %ls", each.temp0Map.c_str());
			CreateComboByTexture(std::string{ "##" } + std::string{ each.materialName.begin(),each.materialName.end() } + std::string{ "Temp3Map" },
				each.temp3Map, textureList);


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