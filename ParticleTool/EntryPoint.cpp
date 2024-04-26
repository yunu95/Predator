#include "YunutyEngine.h"

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "ImGuizmo/ImGuizmo.h"
#include "imgui_Utility.h"
#include "TestUtilGraphicsTestCam.h"
#include "ParticleTool_Manager.h"
#include "FileSystem.h"
#include "ParticleToolData.h"
#include "EditorMath.h"

#include <DirectXMath.h>
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
#include <fstream>
#include <iostream>

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

bool isParticleEditMode = true;

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

void ShowParticleList();
void ShowSkinnedFBXList();
void ShowParticleEditor();
void ShowSequencerEditor();

void LoadResourcesRecursively();

void ImGuiUpdate();
void DrawMenuBar();

void InputUpdate();

void LoadPP();
void SavePP();
void SaveAsPP();

void LoadPPIs();
void SavePPIs();
void SaveAsPPIs();

bool ShowFBXNode(yunutyEngine::GameObject* target);

bool isRunning = true;

const int bufferSize = 255;
static char* particleDataNameBuffer = new char[bufferSize];

/// Gizmo
ImGuizmo::OPERATION operation = (ImGuizmo::OPERATION)0;
ImGuizmo::MODE mode = ImGuizmo::LOCAL;

void ImGui_UpdateEditableDataWTM(const std::weak_ptr<application::particle::ParticleToolInstance>& target, const yunuGI::Matrix4x4& wtm);
///

void ImGui_DrawTransform(int& idx);

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

    application::fileSystem::SetHWND(g_Toolhwnd);

    /// Contents
    LoadResourcesRecursively();
    application::particle::ParticleTool_Manager::GetSingletonInstance().LoadPP("InWanderLand.pp");
    application::particle::ParticleTool_Manager::GetSingletonInstance().LoadSkinnedFBX();
    ///

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

    static auto& pm = application::particle::ParticleTool_Manager::GetSingletonInstance();

    {
        ImGui::SetNextWindowSize(dockspaceArea);
        ImGui::SetNextWindowPos(dockspaceStartPoint);

        InputUpdate();

        ImGui::Begin("ParticleToolDockSpace", nullptr, window_flags);

        // Dockspace
        ImGui::DockSpace(ImGui::GetID("ParticleToolDockSpace"));

        DrawMenuBar();

        {
            ImGui::Begin("List##Particle");

            if (isParticleEditMode)
            {
                ShowParticleList();
            }
            else
            {
                ShowSkinnedFBXList();
            }

            ImGui::End();
        }

        {
            ImGui::Begin("Edit Data##Particle");

            if (isParticleEditMode)
            {
                ShowParticleEditor();
            }
            else
            {
                ShowSequencerEditor();
            }

            ImGui::End();
        }

        {
            ImGui::Begin("RenderImage##Particle");

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

            auto pos = ImGui::GetCursorPos();

            ImGui::Image(
                reinterpret_cast<ImTextureID>(yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager()->GetFinalRenderImage()),
                ImVec2(newRegion.x, newRegion.y)
            );

            ImGuizmo::SetDrawlist();
            float left = ImGui::GetWindowPos().x + pos.x;
            float top = ImGui::GetWindowPos().y + pos.y;
            float right = left + newRegion.x;
            float bottom = top + newRegion.y;
            ImGuizmo::SetRect(left, top, newRegion.x, newRegion.y);
            ImGui::PushClipRect(ImVec2(left, top), ImVec2(right, bottom), true);

            auto cam = yunutyEngine::graphics::Camera::GetMainCamera();

            auto vtm = application::editor::math::GetInverseMatrix(cam->GetTransform()->GetWorldTM());

            float width;
            float height;
            cam->GetGI().GetResolution(&width, &height);
            DirectX::XMMATRIX prm = DirectX::XMMatrixPerspectiveFovLH(cam->GetGI().GetVerticalFOV(), width / height, cam->GetGI().GetNear(), cam->GetGI().GetFar());
            auto ptm = *reinterpret_cast<yunuGI::Matrix4x4*>(&prm);

            yunuGI::Matrix4x4 im = yunuGI::Matrix4x4();

            auto gvtm = application::editor::math::ConvertVTM(vtm);
            auto gptm = application::editor::math::ConvertPTM(ptm);

            auto beforeVTM = gvtm;

            if (!pm.GetSelectedParticleInstanceData().expired())
            {
                auto pi = pm.GetSelectedParticleInstanceData();
                auto pobj = pm.GetParticleToolInstanceObject(pm.GetSelectedParticleInstanceData());

                yunuGI::Matrix4x4 tm = yunuGI::Matrix4x4();
                Vector3d startPosition = Vector3d();

                tm = pobj->GetTransform()->GetWorldTM();

                auto objgwtm = application::editor::math::ConvertWTM(tm);
                if (ImGuizmo::Manipulate(reinterpret_cast<float*>(&beforeVTM), reinterpret_cast<float*>(&gptm), operation, mode, reinterpret_cast<float*>(&objgwtm), NULL, NULL, NULL, NULL))
                {
                    ImGui_UpdateEditableDataWTM(pi, application::editor::math::ConvertWTM(objgwtm));
                }

                ImGui::PopClipRect();
            }

            ImGui::End();
        }

        application::editor::imgui::RenderMessageBoxes();
 
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

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4());

    static auto& pm = application::particle::ParticleTool_Manager::GetSingletonInstance();

    if (ImGui::BeginMenu("Mode"))
    {
        if (ImGui::MenuItem("Particle Edit Mode", 0, isParticleEditMode))
        {
            if (!isParticleEditMode)
            {
                isParticleEditMode = true;
                pm.SwitchMode();
            }
        }

        if (ImGui::MenuItem("Sequencer Mode", 0, !isParticleEditMode))
        {
            if (isParticleEditMode)
            {
                isParticleEditMode = false;
                pm.SwitchMode();
            }
        }

        ImGui::EndMenu();
    }

    if (isParticleEditMode)
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Load"))
            {
                LoadPP();
            }
            if (ImGui::MenuItem("Save As"))
            {
                SaveAsPP();
            }
            if (ImGui::MenuItem("Save", 0, false, !pm.GetCurrentPPPath().empty()))
            {
                SavePP();
            }

            ImGui::EndMenu();
        }
    }
    else
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Load"))
            {
                LoadPPIs();
            }
            if (ImGui::MenuItem("Save As"))
            {
                SaveAsPPIs();
            }
            if (ImGui::MenuItem("Save", 0, false, !pm.GetCurrentPPIsPath().empty()))
            {
                SavePPIs();
            }

            ImGui::EndMenu();
        }
    }

    if (ImGui::Button("Camera Reset"))
    {
        tests::GraphicsTestCam* cts = static_cast<tests::GraphicsTestCam*>(yunutyEngine::graphics::Camera::GetMainCamera());
        cts->Reset();
    }

    ImGui::Text(" | Use IBL : ");
    if (ImGui::Checkbox("##Use IBL", &g_useIBL))
    {
        yunutyEngine::graphics::Renderer::SingleInstance().SetUseIBL(g_useIBL);
    }

    if (isParticleEditMode)
    {
        if (!pm.GetCurrentPPPath().empty())
        {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(pm.GetCurrentPPPath().c_str()).x - 10);
            ImGui::Text(pm.GetCurrentPPPath().c_str());
        }
    }
    else
    {
        if (!pm.GetCurrentPPIsPath().empty())
        {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(pm.GetCurrentPPIsPath().c_str()).x - 10);
            ImGui::Text(pm.GetCurrentPPIsPath().c_str());
        }
    }

    ImGui::PopStyleColor();
    ImGui::EndMenuBar();
}

void ShowParticleList()
{
    static auto& pm = application::particle::ParticleTool_Manager::GetSingletonInstance();

    if (ImGui::Button("Create Particle", ImVec2(ImGui::GetContentRegionAvail().x, 20)))
    {
        memset(particleDataNameBuffer, 0, bufferSize);
        application::editor::imgui::ShowMessageBox("Create Particle", []()
            {
                application::editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));
                
                ImGui::SetNextItemWidth(-1);
                ImGui::InputTextWithHint("##new_particle_name", "Particle Data Name", particleDataNameBuffer, bufferSize);

                ImGui::Separator();

                if (ImGui::Button("Create"))
                {
                    if (*particleDataNameBuffer != '\0')
                    {
                        auto particleName = std::string(particleDataNameBuffer);
                        if (pm.CreateParticle(particleName).lock())
                        {
                            memset(particleDataNameBuffer, 0, bufferSize);
                            ImGui::CloseCurrentPopup();
                            application::editor::imgui::CloseMessageBox("Create Particle");
                        }
                    }
                }
                ImGui::SameLine();

                if (ImGui::Button("Cancel"))
                {
                    memset(particleDataNameBuffer, 0, bufferSize);
                    ImGui::CloseCurrentPopup();
                    application::editor::imgui::CloseMessageBox("Create Particle");
                }
            }, 600);
    }

    auto& pList = pm.GetParticleList();
    std::vector<std::string> selections = std::vector<std::string>();
    for (auto& each : pList)
    {
        selections.push_back(each.lock()->name);
    }

    application::editor::imgui::ShiftCursorY(20);
    application::editor::imgui::draw::Underline();
    for (int i = 0; i < pList.size(); i++)
    {
        if (ImGui::Selectable(selections[i].c_str(), pList[i].lock() == pm.GetSelectedParticleData().lock()))
        {
            if (pm.GetSelectedParticleData().lock() == pList[i].lock())
            {
                pm.SetSelectedParticleData(std::shared_ptr<application::particle::ParticleToolData>());
            }
            else
            {
                pm.SetSelectedParticleData(pList[i].lock());
            }
        }
    }
}

void ShowSkinnedFBXList()
{
    using namespace application::editor::imgui;

    static auto& pm = application::particle::ParticleTool_Manager::GetSingletonInstance();
    
    auto& fbxList = pm.GetSkinnedFBXList();
    std::vector<std::string> selections = std::vector<std::string>();
    for (auto& each : fbxList)
    {
        selections.push_back(each->getName());
    }

    bool rightClickPopup = false;

    for (int i = 0; i < selections.size(); i++)
    {
        rightClickPopup |= ShowFBXNode(fbxList[i]);
    }

    if (rightClickPopup)
    {
        ImGui::OpenPopup("##FBX_Popup");
    }

    if (ImGui::BeginPopup("##FBX_Popup"))
    {
        if (ImGui::MenuItem("Add Particle From Template"))
        {
            application::editor::imgui::ShowMessageBox("From Particle", []()
                {
                    application::editor::imgui::SmartStyleVar padding(ImGuiStyleVar_FramePadding, ImVec2(10, 7));

                    ImGui::Separator();

                    static std::string particleName = "None";

                    ImGui::SetNextItemWidth(-1);
                    if (ImGui::BeginCombo("##ParticleListCombo", particleName.c_str()))
                    {
                        for (auto& each : pm.GetParticleList())
                        {
                            const bool is_selected = (particleName == each.lock()->name);
                            if (ImGui::Selectable(each.lock()->name.c_str(), is_selected))
                            {
                                particleName = each.lock()->name;
                            }

                            if (is_selected)
                            {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }

                    ImGui::Separator();

                    if (ImGui::Button("Create"))
                    {
                        if (particleName != "None")
                        {
                            pm.AddParticleInstance(pm.GetSelectedFBXData(), particleName);
                            particleName = "None";
                            ImGui::CloseCurrentPopup();
                            application::editor::imgui::CloseMessageBox("From Particle");
                        }
                    }
                    ImGui::SameLine();

                    if (ImGui::Button("Cancel"))
                    {
                        particleName = "None";
                        ImGui::CloseCurrentPopup();
                        application::editor::imgui::CloseMessageBox("From Particle");
                    }
                }, 300);
        }

        if (ImGui::MenuItem("Add New Particle"))
        {
            pm.AddParticleInstance(pm.GetSelectedFBXData());
        }

        ImGui::EndPopup();
    }
}

bool ShowFBXNode(yunutyEngine::GameObject* target)
{
    static auto& pm = application::particle::ParticleTool_Manager::GetSingletonInstance();

    bool rightClickPopup = false;

    bool isSelected = (target == pm.GetSelectedFBXData());

    if (isSelected)
    {
        auto pos = ImGui::GetCurrentWindow()->DC.CursorPos;
        ImGui::RenderFrame(pos, ImVec2(pos.x + ImGui::GetContentRegionAvail().x, pos.y + ImGui::CalcTextSize("Font").y), ImGui::GetColorU32(ImGuiCol_HeaderActive), false, 0.0f);
    }

    if (ImGui::TreeNode(target->getName().c_str()))
    {
        if (ImGui::IsItemClicked() && isSelected)
        {
            pm.SetSelectedFBXData(nullptr);
        }

        if (isSelected && ImGui::IsItemClicked(ImGuiMouseButton_Right))
        {
            rightClickPopup = true;
        }

        for (auto& each : pm.GetChildrenParticleInstanceList(target->getName()))
        {
            bool selected = (pm.GetSelectedParticleInstanceData().lock() == each.lock());
            if (ImGui::Selectable(each.lock()->name.c_str(), selected))
            {
                if (isSelected)
                {
                    if (selected)
                    {
                        pm.SetSelectedParticleInstanceData(std::shared_ptr<application::particle::ParticleToolInstance>());
                    }
                    else
                    {
                        pm.SetSelectedParticleInstanceData(each);
                    }
                }
            }
        }

        ImGui::TreePop();
    }
    else
    {
        if (ImGui::IsItemClicked())
        {
            pm.SetSelectedFBXData(target);
        }
    }

    return rightClickPopup;
}

void ShowParticleEditor()
{
    using namespace application::editor::imgui;
    
    static auto& pm = application::particle::ParticleTool_Manager::GetSingletonInstance();

    if (!pm.GetSelectedParticleData().expired())
    {
        int idx = 0;
        auto particleData = pm.GetSelectedParticleData().lock();
        if (BeginSection_2Col(idx, "Particle Data", ImGui::GetContentRegionAvail().x, 0.3))
        {
            {
                std::string particleName = particleData->name;
                particleName.reserve(32);
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                SmartStyleColor textColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 255));
                ImGui::AlignTextToFramePadding();
                ImGui::Text("Name");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-1);
                if (ImGui::InputText("##Particle_Name", &particleName[0], 32))
                {
                    int strSize = MultiByteToWideChar(CP_UTF8, 0, particleName.c_str(), -1, nullptr, 0) - 1;
                    particleName.resize(strSize);
                    pm.RenameParticleData(particleData, particleName);
                }
            }

            static const char* shapeList[2] = { "Cone", "Circle" };
            int selectedShape = (int)particleData->shape;
            if (Dropdown_2Col("Shape", shapeList, 2, &selectedShape))
            {
                particleData->shape = (application::particle::ParticleShape)selectedShape;
            }

            static const char* modeList[2] = { "Default", "Bursts" };
            int selectedMode = (int)particleData->particleMode;
            if (Dropdown_2Col("Mode", modeList, 2, &selectedMode))
            {
                particleData->particleMode = (application::particle::ParticleMode)selectedMode;
            }

            Checkbox_2Col("Loop", particleData->isLoop);
            DragFloat_2Col("Life Time", particleData->lifeTime);
            DragFloat_2Col("Speed", particleData->speed);
            DragFloat_2Col("Start Scale", particleData->startScale);
            DragFloat_2Col("End Scale", particleData->endScale);

            int maxParticle = particleData->maxParticle;
            if (DragInt_2Col("Max Particle", maxParticle, true, 1.f, 1, 500))
            {
                particleData->maxParticle = maxParticle;
            }

            Checkbox_2Col("Play Awake", particleData->playAwake);

            switch (particleData->particleMode)
            {
                case application::particle::ParticleMode::Default:
                {
                    DragFloat_2Col("Rate OverTime", particleData->rateOverTime);
                    break;
                }
                case application::particle::ParticleMode::Bursts:
                {
                    DragInt_2Col("Bursts Count", particleData->burstsCount);
                    DragFloat_2Col("Interval", particleData->interval);
                    break;
                }
                default:
                    break;
            }

            pm.UpdateParticleDataObj(particleData->name);

            EndSection();
        }
    }
}

void ShowSequencerEditor()
{
    using namespace application::editor::imgui;

    static auto& pm = application::particle::ParticleTool_Manager::GetSingletonInstance();

    if (!pm.GetSelectedParticleInstanceData().expired())
    {
        int idx = 0;
        auto particleDataInstance = pm.GetSelectedParticleInstanceData().lock();
        auto& particleData = particleDataInstance->particleData;

        if (BeginSection_2Col(idx, "Particle Instance Data", ImGui::GetContentRegionAvail().x, 0.3))
        {
            {
                std::string particleInstanceName = particleDataInstance->name;
                particleInstanceName.reserve(32);
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                SmartStyleColor textColor(ImGuiCol_Text, IM_COL32(180, 180, 180, 255));
                ImGui::AlignTextToFramePadding();
                ImGui::Text("Name");
                ImGui::TableSetColumnIndex(1);
                ImGui::SetNextItemWidth(-1);
                if (ImGui::InputText("##Particle_Ins_Name", &particleInstanceName[0], 32))
                {
                    int strSize = MultiByteToWideChar(CP_UTF8, 0, particleInstanceName.c_str(), -1, nullptr, 0) - 1;
                    particleInstanceName.resize(strSize);
                    particleDataInstance->name = particleInstanceName;
                }
            }

            static const char* shapeList[2] = { "Cone", "Circle" };
            int selectedShape = (int)particleData.shape;
            if (Dropdown_2Col("Shape", shapeList, 2, &selectedShape))
            {
                particleData.shape = (application::particle::ParticleShape)selectedShape;
            }

            static const char* modeList[2] = { "Default", "Bursts" };
            int selectedMode = (int)particleData.particleMode;
            if (Dropdown_2Col("Mode", modeList, 2, &selectedMode))
            {
                particleData.particleMode = (application::particle::ParticleMode)selectedMode;
            }

            Checkbox_2Col("Loop", particleData.isLoop);
            DragFloat_2Col("Life Time", particleData.lifeTime);
            DragFloat_2Col("Speed", particleData.speed);
            DragFloat_2Col("Start Scale", particleData.startScale);
            DragFloat_2Col("End Scale", particleData.endScale);

            int maxParticle = particleData.maxParticle;
            if (DragInt_2Col("Max Particle", maxParticle, true, 1.f, 1, 500))
            {
                particleData.maxParticle = maxParticle;
            }

            Checkbox_2Col("Play Awake", particleData.playAwake);

            switch (particleData.particleMode)
            {
                case application::particle::ParticleMode::Default:
                {
                    DragFloat_2Col("Rate OverTime", particleData.rateOverTime);
                    break;
                }
                case application::particle::ParticleMode::Bursts:
                {
                    DragInt_2Col("Bursts Count", particleData.burstsCount);
                    DragFloat_2Col("Interval", particleData.interval);
                    break;
                }
                default:
                    break;
            }

            pm.UpdateParticleInstanceDataObj(particleDataInstance);

            EndSection();
        }

        ImGui_DrawTransform(idx);
    }
}

void InputUpdate()
{
    static auto& pm = application::particle::ParticleTool_Manager::GetSingletonInstance();

    if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
    {
        if (ImGui::IsKeyPressed(ImGuiKey_S, false))
        {
            if (isParticleEditMode)
            {
                if (!pm.GetCurrentPPPath().empty())
                {
                    SaveAsPP();
                }
                else
                {
                    SavePP();
                }
            }
            else
            {
                if (!pm.GetCurrentPPIsPath().empty())
                {
                    SaveAsPPIs();
                }
                else
                {
                    SavePPIs();
                }
            }
        }
    }

    if (!ImGui::IsMouseDown(ImGuiMouseButton_Right))
    {
        if (ImGui::IsKeyPressed(ImGuiKey_Q))
        {
            operation = (ImGuizmo::OPERATION)0;
        }
        else if (ImGui::IsKeyPressed(ImGuiKey_W))
        {
            operation = ImGuizmo::OPERATION::TRANSLATE;
        }
        else if (ImGui::IsKeyPressed(ImGuiKey_E))
        {
            operation = ImGuizmo::OPERATION::ROTATE;
        }
        else if (ImGui::IsKeyPressed(ImGuiKey_R))
        {
            operation = ImGuizmo::OPERATION::SCALE;
        }
    }
}

void LoadPP()
{
    std::filesystem::path filepath = application::fileSystem::LoadFileDialog("Particle File (*.pp)\0*.pp\0");

    if (filepath.empty())
        return;

    if (!filepath.has_extension())
        filepath += ".pp";

    application::particle::ParticleTool_Manager::GetSingletonInstance().LoadPP(filepath.string());
}

void SavePP()
{
    static auto& pm =  application::particle::ParticleTool_Manager::GetSingletonInstance();
    pm.SavePP(pm.GetCurrentPPPath());
}

void SaveAsPP()
{
    static auto& pm = application::particle::ParticleTool_Manager::GetSingletonInstance();
    
    std::filesystem::path filepath = application::fileSystem::SaveFileDialog("Particle File (*.pp)\0*.pp\0");

    if (filepath.empty())
        return;

    if (!filepath.has_extension())
        filepath += ".pp";

    pm.SetCurrentPPPath(filepath.string());
    pm.SavePP(filepath.string());
}

void LoadPPIs()
{
    std::filesystem::path filepath = application::fileSystem::LoadFileDialog("Particle Instances File (*.ppis)\0*.ppis\0");

    if (filepath.empty())
        return;

    if (!filepath.has_extension())
        filepath += ".ppis";

    application::particle::ParticleTool_Manager::GetSingletonInstance().LoadPPIs(filepath.string());
}

void SavePPIs()
{
    static auto& pm = application::particle::ParticleTool_Manager::GetSingletonInstance();
    pm.SavePPIs(pm.GetCurrentPPIsPath());
}

void SaveAsPPIs()
{
    static auto& pm = application::particle::ParticleTool_Manager::GetSingletonInstance();

    std::filesystem::path filepath = application::fileSystem::SaveFileDialog("Particle Instances File (*.ppis)\0*.ppis\0");

    if (filepath.empty())
        return;

    if (!filepath.has_extension())
        filepath += ".ppis";

    pm.SetCurrentPPIsPath(filepath.string());
    pm.SavePPIs(filepath.string());
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
            auto directorList = application::fileSystem::GetSubdirectories("FBX");
            for (auto each : directorList)
            {
                resourceManager->LoadFile(("FBX/" + each.string()).c_str());
            }
        }
    }
}

void ImGui_UpdateEditableDataWTM(const std::weak_ptr<application::particle::ParticleToolInstance>& target, const yunuGI::Matrix4x4& wtm)
{
    static auto& pm = application::particle::ParticleTool_Manager::GetSingletonInstance();

    yunuGI::Vector3 scale;
    yunuGI::Quaternion rotation;
    yunuGI::Vector3 translation;
    application::editor::math::DecomposeWTM(wtm, scale, rotation, translation);

    target.lock()->offsetPos = *reinterpret_cast<Vector3f*>(&translation);
    target.lock()->rotation = *reinterpret_cast<Quaternion*>(&rotation);
    target.lock()->scale = *reinterpret_cast<Vector3f*>(&scale);

    pm.UpdateParticleInstanceDataObj(target);
}

void ImGui_DrawTransform(int& idx)
{
    static auto& pm = application::particle::ParticleTool_Manager::GetSingletonInstance();

    auto particleDataInstance = pm.GetSelectedParticleInstanceData().lock();
    auto& particleData = particleDataInstance->particleData;
    auto pobj = pm.GetParticleToolInstanceObject(pm.GetSelectedParticleInstanceData());

    static bool isEditing = false;

    if (application::editor::imgui::BeginSection_1Col(idx, "TransForm", ImGui::GetContentRegionAvail().x))
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        Vector3f position = Vector3f();
        Vector3f rotation = Vector3f();
        Vector3f scale = Vector3f(1, 1, 1);

        bool reset[9] = { false };
        bool endEdit = false;

        position = pobj->GetTransform()->GetLocalPosition();
        rotation = pobj->GetTransform()->GetLocalRotation().Euler();
        scale = pobj->GetTransform()->GetLocalScale();

        auto resetPosition = application::editor::imgui::Vector3Control("Position", position.x, position.y, position.z);
        auto resetRotation = application::editor::imgui::Vector3Control("Rotation", rotation.x, rotation.y, rotation.z);
        auto resetScale = application::editor::imgui::Vector3Control("Scale", scale.x, scale.y, scale.z);

        switch (resetPosition)
        {
            case application::editor::imgui::Vector3Flags::ResetX:
                reset[0] = true;
                break;
            case application::editor::imgui::Vector3Flags::ResetY:
                reset[1] = true;
                break;
            case application::editor::imgui::Vector3Flags::ResetZ:
                reset[2] = true;
                break;
            case application::editor::imgui::Vector3Flags::EndEditX:
                endEdit = true;
                break;
            case application::editor::imgui::Vector3Flags::EndEditY:
                endEdit = true;
                break;
            case application::editor::imgui::Vector3Flags::EndEditZ:
                endEdit = true;
                break;
            default:
                break;
        }

        switch (resetRotation)
        {
            case application::editor::imgui::Vector3Flags::ResetX:
                reset[3] = true;
                break;
            case application::editor::imgui::Vector3Flags::ResetY:
                reset[4] = true;
                break;
            case application::editor::imgui::Vector3Flags::ResetZ:
                reset[5] = true;
                break;
            case application::editor::imgui::Vector3Flags::EndEditX:
                endEdit = true;
                break;
            case application::editor::imgui::Vector3Flags::EndEditY:
                endEdit = true;
                break;
            case application::editor::imgui::Vector3Flags::EndEditZ:
                endEdit = true;
                break;
            default:
                break;
        }

        switch (resetScale)
        {
            case application::editor::imgui::Vector3Flags::ResetX:
                reset[6] = true;
                break;
            case application::editor::imgui::Vector3Flags::ResetY:
                reset[7] = true;
                break;
            case application::editor::imgui::Vector3Flags::ResetZ:
                reset[8] = true;
                break;
            case application::editor::imgui::Vector3Flags::EndEditX:
                endEdit = true;
                break;
            case application::editor::imgui::Vector3Flags::EndEditY:
                endEdit = true;
                break;
            case application::editor::imgui::Vector3Flags::EndEditZ:
                endEdit = true;
                break;
            default:
                break;
        }

        if (reset[0])
        {
            position.x = 0;
        }
        else if (reset[1])
        {
            position.y = 0;
        }
        else if (reset[2])
        {
            position.z = 0;
        }
        else if (reset[3])
        {
            rotation.x = 0;
        }
        else if (reset[4])
        {
            rotation.y = 0;
        }
        else if (reset[5])
        {
            rotation.z = 0;
        }
        else if (reset[6])
        {
            scale.x = 1;
        }
        else if (reset[7])
        {
            scale.y = 1;
        }
        else if (reset[8])
        {
            scale.z = 1;
        }

        if (isEditing == false &&
            (position != Vector3f(pobj->GetTransform()->GetLocalPosition()) ||
                rotation != Vector3f(pobj->GetTransform()->GetLocalRotation().Euler()) ||
                scale != Vector3f(pobj->GetTransform()->GetLocalScale())))
        {
            isEditing = true;
        }

        particleDataInstance->offsetPos = position;
        particleDataInstance->rotation = Quaternion(rotation);

        if (scale.x == 0)
        {
            scale.x = 0.000001;
        }
        if (scale.y == 0)
        {
            scale.y = 0.000001;
        }
        if (scale.z == 0)
        {
            scale.z = 0.000001;
        }

        particleDataInstance->scale = scale;
        pm.UpdateParticleInstanceDataObj(particleDataInstance);

        for (auto each : reset)
        {
            endEdit |= each;
        }

        if (endEdit)
        {
            isEditing = false;
        }

        application::editor::imgui::EndSection();
    }
}
