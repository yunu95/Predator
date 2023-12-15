#include "Application.h"

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#include "YunutyEngine.h"
#include "EditorLayer.h"
#include "ContentsLayer.h"

#include <d3d11.h>
#include <tchar.h>

#include <cassert>

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
ID3D11ShaderResourceView* GetSRV(void* handle);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Data
ID3D11Device* g_pd3dDevice = NULL;
ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
IDXGISwapChain* g_pSwapChain = NULL;
UINT g_ResizeWidth = NULL;
UINT g_ResizeHeight = NULL;
ID3D11RenderTargetView* g_mainRenderTargetView = NULL;
HWND hWND = NULL;
HWND editorHWND = NULL;
WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("InWanderLand"), NULL };

namespace Application
{
    std::unique_ptr<Application> Application::instance = nullptr;

    Application& Application::CreateApplication(int argc, char** argv)
    {
        if (instance == nullptr)
        {
            instance = std::unique_ptr<Application>(new Application(argc, argv));
        }

        return *instance;
    }

    Application& Application::GetInstance()
    {
        assert(instance && "You must first create an Application.");

        return *instance;
    }

    Application::~Application()
    {

    }

    Application::Application(int argc, char** argv)
    {
        // Create application window
        //ImGui_ImplWin32_EnableDpiAwareness();
        ::RegisterClassEx(&wc);

        /// 게임 윈도우 생성
        int winSizeX = 1920;	// 윈도우 가로 사이즈
        int winSizeY = 1080;	// 윈도우 세로 사이즈
        int winPosX = (GetSystemMetrics(SM_CXSCREEN) - winSizeX) / 2;	// 윈도우 X 좌표
        int winPosY = (GetSystemMetrics(SM_CYSCREEN) - winSizeY) / 2;	// 윈도우 Y 좌표
        hWND = ::CreateWindow(wc.lpszClassName, _T("InWanderLand"), WS_OVERLAPPEDWINDOW, winPosX, winPosY, winSizeX, winSizeY, NULL, NULL, wc.hInstance, NULL);

        // Initialize Direct3D
        if (!CreateDeviceD3D(hWND))
        {
            CleanupDeviceD3D();
            ::UnregisterClass(wc.lpszClassName, wc.hInstance);

            throw std::runtime_error(std::string("failed to create d3d device!"));
        }

        ::ShowWindow(hWND, SW_SHOWDEFAULT);
        ::UpdateWindow(hWND);


#ifdef _DEBUG
        /// 에디터 윈도우 생성
        int editorWinSizeX = 1280;
        int editorWinSizeY = 800;
        int editorWinPosX = (GetSystemMetrics(SM_CXSCREEN) - editorWinSizeX) / 2 + 500;
        int editorWinPosY = (GetSystemMetrics(SM_CYSCREEN) - editorWinSizeY) / 2 - 300;
        editorHWND = ::CreateWindow(wc.lpszClassName, _T("Editor Window"), WS_OVERLAPPEDWINDOW, editorWinPosX, editorWinPosY, editorWinSizeX, editorWinSizeY, NULL, NULL, wc.hInstance, NULL);

        //::ShowWindow(editorHWND, SW_SHOWDEFAULT);
        //::UpdateWindow(editorHWND);

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
        ImGui_ImplWin32_Init(editorHWND);
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
#endif

        /// Contents
        //yunutyEngine::graphics::Renderer::SingleInstance().LoadGraphicsDll(L"YunuDX11Renderer.dll");
        //yunutyEngine::graphics::Renderer::SingleInstance().LoadGraphicsDll(L"MZDX11Renderer.dll");
        yunutyEngine::graphics::Renderer::SingleInstance().LoadGraphicsDll(L"NailEngine.dll");
        yunutyEngine::graphics::Renderer::SingleInstance().SetResolution(winSizeX, winSizeY);
        yunutyEngine::graphics::Renderer::SingleInstance().SetOutputWindow(hWND);
    }

    void Application::Initialize()
    {
        layers.resize(2);

        //ImGui::SetCursorPosY(ImGui::GetCurrentWindow()->WindowPadding.y);

#ifdef _DEBUG
        layers[(int)LayerList::EditorLayer] = new Editor::EditorLayer();
#endif

        layers[(int)LayerList::ContentsLayer] = new Contents::ContentsLayer();

        for (auto each : layers)
        {
            each->Initialize();
        }
    }

    void Application::Run()
    {
        isRunning = true;
        while (isRunning)
        {
            MSG msg;
            while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
            {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
                if (msg.message == WM_QUIT)
                    isRunning = false;
            }
            if (!isRunning)
                break;

            /// Editor 관련 영역, Release Mode 에서는 구현 안되도록 처리
#ifdef _DEBUG
        //Start the Dear ImGui frame
            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
            //window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
            window_flags |= ImGuiWindowFlags_MenuBar;

            {
                ImGui::Begin("DockSpace Demo", nullptr, window_flags);

                ImGuiStyle& style = ImGui::GetStyle();

                // Dockspace
                float minWinSizeX = style.WindowMinSize.x;
                style.WindowMinSize.x = 370.0f;
                ImGui::DockSpace(ImGui::GetID("MyDockspace"));
                style.WindowMinSize.x = minWinSizeX;

                layers[(int)LayerList::EditorLayer]->Update(1);
                layers[(int)LayerList::EditorLayer]->GUIProgress();

                ImGui::End();
            }

            ImGui::Render();

            ImVec4 clear_color = ImVec4(0.8f, 0.2f, 0.2f, 1.00f);
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

            //g_pSwapChain->Present(1, 0); // Present with vsync
            g_pSwapChain->Present(0, 0); // Present without vsync

            // 커맨드들 실행
            cm.ExecuteCommands();
#endif

            layers[(int)LayerList::ContentsLayer]->Update(1);
            layers[(int)LayerList::ContentsLayer]->GUIProgress();

            // 게임 엔진을 멈추고 동작을 실행하는 부분
            {
                std::scoped_lock lock{loopTodoRegistrationMutex};
                if (!loopRegistrations.empty())
                {
                    std::unique_lock preupdateLock{YunutyCycle::SingleInstance().preUpdateMutex};
                    std::unique_lock updateLock{YunutyCycle::SingleInstance().updateMutex};

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
    }

    void Application::Finalize()
    {
        for (auto each : layers)
        {
            each->Finalize();

            delete each;
        }

#ifdef _DEBUG
        // Cleanup
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
#endif

        CleanupDeviceD3D();
        ::DestroyWindow(hWND);

#ifdef _DEBUG
        ::DestroyWindow(editorHWND);
#endif
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
    }

    void Application::TurnOff()
    {
        if (isRunning)
        {
            isRunning = false;
        }
    }
    void Application::AddMainLoopTodo(std::function<void()> todo)
    {
        scoped_lock lock{ loopTodoRegistrationMutex };
        loopRegistrations.push_back(todo);
    }
}

// Helper functions
bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);

    if (res != S_OK)
    {
        std::stringstream ss;
        ss << std::hex << res;
        throw std::runtime_error(string("D3D11CreateDeviceAndSwapChain 함수에서 런타임 오류가 났습니다. 에러 코드 : ") + ss.str());
    }

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

ID3D11ShaderResourceView* GetSRV(void* handle)
{
    static std::map<void*, ID3D11ShaderResourceView*> srvs;
    if (srvs.find(handle) == srvs.end())
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc
        {
            .Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
            .ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
            .Texture2D
            {
                .MostDetailedMip = 0,
                .MipLevels = 1,
            },
        };
        ID3D11Resource* receivedResource{ nullptr };
        ID3D11ShaderResourceView* srv{ nullptr };
        g_pd3dDevice->OpenSharedResource(handle, IID_PPV_ARGS(&receivedResource));
        g_pd3dDevice->CreateShaderResourceView(receivedResource, &srvDesc, &srv);
        srvs[handle] = srv;
    }
    return srvs[handle];
    }

#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0 // From Windows SDK 8.1+ headers
#endif

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    case WM_DPICHANGED:
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
        {
            //const int dpi = HIWORD(wParam);
            //printf("WM_DPICHANGED to %d (%.0f%%)\n", dpi, (float)dpi / 96.0f * 100.0f);
            const RECT* suggested_rect = (RECT*)lParam;
            ::SetWindowPos(hWnd, nullptr, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
        }
        break;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
