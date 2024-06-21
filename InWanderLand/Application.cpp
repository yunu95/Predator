#include "InWanderLand.h"
#include "Application.h"

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "ImGuizmo/ImGuizmo.h"

#include "YunutyEngine.h"
#include "EditorLayer.h"
#include "ContentsLayer.h"
#include "EditorCommonEvents.h"
#include "WindowEvents.h"
#include "MouseEvents.h"
#include "KeyboardEvents.h"
#include "PaletteManager.h"
#include "Palette.h"
#include "EditorCamera.h"
#include "InstanceManager.h"
#include "TemplateDataManager.h"
#include "PaletteBrushManager.h"
#include "EditorCameraManager.h"
#include "EditorCamera.h"
#include "PlayableComponent.h"
#include "WanderUtils.h"
#include "UnitPool.h"

#include <d3d11.h>
#include <dxgi1_4.h>
#include <tchar.h>

#include <cassert>

#include <windowsx.h>

// Forward declarations of helper functions
void GetDeviceAndDeviceContext();
void CreateRenderTarget();
bool CreateSwapChain();
void CleanupRenderTarget();
void CleanupSwapChain();
void ResizeBuffers();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndEditorProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Data
UINT g_ResizeWidth = NULL;
UINT g_ResizeHeight = NULL;
HWND hWND = NULL;
WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("InWanderLand"), NULL };
ID3D11Device* g_pD3dDevice = NULL;
ID3D11DeviceContext* g_pD3dDeviceContext = NULL;
bool gameFocus = false;

#ifdef EDITOR
IDXGISwapChain1* g_EditorpSwapChain = NULL;
UINT g_EditorResizeWidth = NULL;
UINT g_EditorResizeHeight = NULL;
ID3D11RenderTargetView* g_EditormainRenderTargetView = NULL;
ID3D11ShaderResourceView* g_EditorShaderResourceView = NULL;
HWND editorHWND = NULL;
WNDCLASSEX wcEditor = { sizeof(WNDCLASSEX), CS_CLASSDC, WndEditorProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("InWanderLand_Editor"), NULL };
ImVec2 dockspaceArea = ImVec2();
ImVec2 dockspaceStartPoint = ImVec2();
RAWINPUTDEVICE inputDevice[2] = { RAWINPUTDEVICE(), RAWINPUTDEVICE() };
std::function<void()> gameWindowFocusCallBackFunction = std::function<void()>();
std::function<void()> gameWindowKillFocusCallBackFunction = std::function<void()>();
std::function<void()> winResizeCallBackFunction = std::function<void()>();
std::function<void(unsigned char keyCode)> winKeyboardPressedCallBackFunction = std::function<void(unsigned char)>();
std::function<void(unsigned char keyCode)> winKeyboardUpCallBackFunction = std::function<void(unsigned char)>();
std::function<void()> winMouseLeftPressedCallBackFunction = std::function<void()>();
std::function<void()> winMouseLeftUpCallBackFunction = std::function<void()>();
std::function<void()> winMouseRightPressedCallBackFunction = std::function<void()>();
std::function<void()> winMouseRightUpCallBackFunction = std::function<void()>();
std::function<void(long posX, long posY)> winMouseMoveCallBackFunction = std::function<void(long, long)>();
std::function<void(short wheelDelta)> winMouseWheelCallBackFunction = std::function<void(short)>();
#endif

namespace application
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

    Application& Application::CreateApplicationWinMain(int* hInstance)
    {
        if (instance == nullptr)
        {
            instance = std::unique_ptr<Application>(new Application(hInstance));
        }

        return *instance;
    }

    Application& Application::GetInstance()
    {
        assert(instance && "You must first create an application.");

        return *instance;
    }

    Application::~Application()
    {

    }

    bool Application::IsFocusGameWindow()
    {
        return gameFocus;
    }

    Application::Application(int argc, char** argv)
    {
        // Create application window
        //ImGui_ImplWin32_EnableDpiAwareness();
        ::RegisterClassEx(&wc);

        appSpecification.appName = wc.lpszClassName;
        appSpecification.windowWidth = 1920;
        appSpecification.windowHeight = 1080;

        monitorResolution.width = GetSystemMetrics(SM_CXSCREEN);
        monitorResolution.height = GetSystemMetrics(SM_CYSCREEN);

        /// 게임 윈도우 생성
        int winPosX = (monitorResolution.width - appSpecification.windowWidth) / 2;	// 윈도우 X 좌표
        int winPosY = (monitorResolution.height - appSpecification.windowHeight) / 2;	// 윈도우 Y 좌표

        hWND = ::CreateWindow(wc.lpszClassName, wc.lpszClassName, WS_OVERLAPPEDWINDOW, winPosX, winPosY, appSpecification.windowWidth, appSpecification.windowHeight, NULL, NULL, wc.hInstance, NULL);

        RECT wndRect;
        GetClientRect(hWND, &wndRect);

        int newWidth = 1920 + (1920 - wndRect.right);
        int newHeight = 1080 + (1080 - wndRect.bottom);

        //float desiredRatio = 1920.0f / 1080.0f;

        SetWindowPos(hWND, NULL, 0, 0, newWidth, newHeight, SWP_NOMOVE | SWP_NOZORDER);
#ifndef EDITOR
        LONG_PTR style = GetWindowLongPtr(hWND, GWL_STYLE);
        style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
        SetWindowLongPtr(hWND, GWL_STYLE, style);
#endif

        ::ShowWindow(hWND, SW_SHOWDEFAULT);
        ::UpdateWindow(hWND);

#ifdef EDITOR
        /// 에디터 윈도우 생성
        g_EditorResizeWidth = appSpecification.windowWidth;
        g_EditorResizeHeight = appSpecification.windowHeight;
        int editorWinPosX = monitorResolution.width - (g_EditorResizeWidth / 2);
        int editorWinPosY = (monitorResolution.height - g_EditorResizeHeight) / 2 + 100;

        // 게임엔진 스레드에서 에디터 윈도우를 생성하도록 유도
        yunutyEngine::YunutyCycle::SingleInstance().preThreadAction = [&, editorWinPosX, editorWinPosY, this]()
            {
                ::RegisterClassEx(&wcEditor);

                editorHWND = ::CreateWindow(wcEditor.lpszClassName, wcEditor.lpszClassName, WS_OVERLAPPEDWINDOW, editorWinPosX, editorWinPosY, g_EditorResizeWidth, g_EditorResizeHeight, hWND, NULL, wcEditor.hInstance, NULL);

                GetDeviceAndDeviceContext();
                erm.Initialize(g_pD3dDevice);

                // Initialize Direct3D
                if (!CreateSwapChain())
                {
                    CleanupSwapChain();
                    ::UnregisterClass(wcEditor.lpszClassName, wcEditor.hInstance);

                    throw std::runtime_error(std::string("failed to create d3d device!"));
                }

                // Window 관련 CallBack 구현
                SetWindowCallBack();

                ::ShowWindow(editorHWND, SW_SHOWDEFAULT);
                ::UpdateWindow(editorHWND);

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
                ImGui_ImplWin32_Init(editorHWND);
                ImGui_ImplDX11_Init(g_pD3dDevice, g_pD3dDeviceContext);

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
        yunutyEngine::YunutyCycle::SingleInstance().postUpdateAction = []() { Application::instance->ImGuiUpdate(); };
        yunutyEngine::YunutyCycle::SingleInstance().postThreadAction = []()
            {
                // Cleanup
                ImGui_ImplDX11_Shutdown();
                ImGui_ImplWin32_Shutdown();
                ImGui::DestroyContext();
                CleanupSwapChain();
                ::DestroyWindow(editorHWND);
            };
#endif
        yunutyEngine::graphics::Renderer::SingleInstance().LoadGraphicsDll(L"NailEngine.dll");
        yunutyEngine::graphics::Renderer::SingleInstance().SetResolution(appSpecification.windowWidth, appSpecification.windowHeight);
        yunutyEngine::graphics::Renderer::SingleInstance().SetOutputWindow(hWND);
    }

    Application::Application(int* hInstance)
    {
        // Create application window
         //ImGui_ImplWin32_EnableDpiAwareness();
        wc.hInstance = (HINSTANCE)hInstance;
        ::RegisterClassEx(&wc);

        appSpecification.appName = wc.lpszClassName;
        appSpecification.windowWidth = 1920;
        appSpecification.windowHeight = 1080;

        monitorResolution.width = GetSystemMetrics(SM_CXSCREEN);
        monitorResolution.height = GetSystemMetrics(SM_CYSCREEN);

        /// 게임 윈도우 생성
        int winPosX = (monitorResolution.width - appSpecification.windowWidth) / 2;	// 윈도우 X 좌표
        int winPosY = (monitorResolution.height - appSpecification.windowHeight) / 2;	// 윈도우 Y 좌표

        hWND = ::CreateWindow(wc.lpszClassName, wc.lpszClassName, WS_OVERLAPPEDWINDOW, winPosX, winPosY, appSpecification.windowWidth, appSpecification.windowHeight, NULL, NULL, wc.hInstance, NULL);

        RECT wndRect;
        GetClientRect(hWND, &wndRect);

        int newWidth = 1920 + (1920 - wndRect.right);
        int newHeight = 1080 + (1080 - wndRect.bottom);

        //float desiredRatio = 1920.0f / 1080.0f;

        SetWindowPos(hWND, NULL, 0, 0, newWidth, newHeight, SWP_NOMOVE | SWP_NOZORDER);
#ifndef EDITOR
        LONG_PTR style = GetWindowLongPtr(hWND, GWL_STYLE);
        style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
        SetWindowLongPtr(hWND, GWL_STYLE, style);
#endif
        ::ShowWindow(hWND, SW_SHOWDEFAULT);
        ::UpdateWindow(hWND);

#ifdef EDITOR
        /// 에디터 윈도우 생성
        g_EditorResizeWidth = appSpecification.windowWidth;
        g_EditorResizeHeight = appSpecification.windowHeight;
        int editorWinPosX = monitorResolution.width - (g_EditorResizeWidth / 2);
        int editorWinPosY = (monitorResolution.height - g_EditorResizeHeight) / 2 + 100;

        // 게임엔진 스레드에서 에디터 윈도우를 생성하도록 유도
        yunutyEngine::YunutyCycle::SingleInstance().preThreadAction = [&, editorWinPosX, editorWinPosY, this]()
            {
                ::RegisterClassEx(&wcEditor);

                editorHWND = ::CreateWindow(wcEditor.lpszClassName, wcEditor.lpszClassName, WS_OVERLAPPEDWINDOW, editorWinPosX, editorWinPosY, g_EditorResizeWidth, g_EditorResizeHeight, hWND, NULL, wcEditor.hInstance, NULL);

                GetDeviceAndDeviceContext();
                erm.Initialize(g_pD3dDevice);

                // Initialize Direct3D
                if (!CreateSwapChain())
                {
                    CleanupSwapChain();
                    ::UnregisterClass(wcEditor.lpszClassName, wcEditor.hInstance);

                    throw std::runtime_error(std::string("failed to create d3d device!"));
                }

                // Window 관련 CallBack 구현
                SetWindowCallBack();

                ::ShowWindow(editorHWND, SW_SHOWDEFAULT);
                ::UpdateWindow(editorHWND);

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
                ImGui_ImplWin32_Init(editorHWND);
                ImGui_ImplDX11_Init(g_pD3dDevice, g_pD3dDeviceContext);

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
        yunutyEngine::YunutyCycle::SingleInstance().postUpdateAction = []() { Application::instance->ImGuiUpdate(); };
        yunutyEngine::YunutyCycle::SingleInstance().postThreadAction = []()
            {
                // Cleanup
                ImGui_ImplDX11_Shutdown();
                ImGui_ImplWin32_Shutdown();
                ImGui::DestroyContext();
                CleanupSwapChain();
                ::DestroyWindow(editorHWND);
            };
#endif
        yunutyEngine::graphics::Renderer::SingleInstance().LoadGraphicsDll(L"NailEngine.dll");
        yunutyEngine::graphics::Renderer::SingleInstance().SetResolution(appSpecification.windowWidth, appSpecification.windowHeight);
        yunutyEngine::graphics::Renderer::SingleInstance().SetOutputWindow(hWND);
    }

    void Application::Initialize()
    {
        layers.resize(2);

        layers[(int)LayerList::ContentsLayer] = new contents::ContentsLayer();

#ifdef EDITOR
        layers[(int)LayerList::EditorLayer] = new editor::EditorLayer();
#endif

        layers[(int)LayerList::ContentsLayer]->Initialize();

        // EdtiorLayer::Initialize에 대한 호출은 의존성 문제로
        // ContentsLayer의 내부 코루틴 함수가 부르는 것으로 변경되었습니다.
    }

    void Application::Run()
    {
        isRunning = true;
        yunutyEngine::YunutyCycle::SingleInstance().Play();
        while (isRunning)
        {
            Sleep(10);
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
            Sleep(10);
        }
        if (YunutyCycle::SingleInstance().IsGameRunning())
            YunutyCycle::SingleInstance().Release();
    }

    void Application::Finalize()
    {
#ifdef EDITOR
        for (auto each : layers)
        {
            each->Finalize();

            delete each;
        }
#endif
        yunutyEngine::graphics::Renderer::SingleInstance().Finalize();
        ::DestroyWindow(hWND);
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);

#ifdef EDITOR
        ::DestroyWindow(editorHWND);
        ::UnregisterClass(wcEditor.lpszClassName, wcEditor.hInstance);
#endif

        editor::InstanceManager::GetSingletonInstance().Clear();
        editor::TemplateDataManager::GetSingletonInstance().Clear();
    }

    void Application::PlayContents()
    {
        auto cl = static_cast<contents::ContentsLayer*>(layers[(int)LayerList::ContentsLayer]);
        auto el = static_cast<editor::EditorLayer*>(layers[(int)LayerList::EditorLayer]);

        if (isContentsPlaying)
        {
            cl->ResumeContents();
            el->OnResumeContents();
        }
        else
        {
			isContentsPlaying = true;
			cl->PlayContents();
			el->OnPlayContents();
        }
    }

    void Application::PauseContents()
    {
        static_cast<contents::ContentsLayer*>(layers[(int)LayerList::ContentsLayer])->PauseContents();
        static_cast<editor::EditorLayer*>(layers[(int)LayerList::EditorLayer])->OnPauseContents();
    }

    void Application::StopContents()
    {
        isContentsPlaying = false;
        static_cast<contents::ContentsLayer*>(layers[(int)LayerList::ContentsLayer])->StopContents();
        static_cast<editor::EditorLayer*>(layers[(int)LayerList::EditorLayer])->OnStopContents();
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

    const MonitorResolution& Application::GetMonitorResolution() const
    {
        return monitorResolution;
    }

    const ApplicationSpecification& Application::GetApplicationSpecification() const
    {
        return appSpecification;
    }

    void* Application::GetSceneSRV()
    {
        static auto resourceManager = yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager();
        return resourceManager->GetFinalRenderImage();
    }

    void* Application::GetWindowHandle()
    {
        return hWND;
    }

    void Application::OnDataLoad()
    {
#ifdef EDITOR
        auto el = static_cast<editor::EditorLayer*>(layers[(int)LayerList::EditorLayer]);
        el->ReadyOrnament();
        el->CreateDirectionalLight();
        editor::EditorCamera::GetSingletonInstance().ReloadGameCamera();
        editor::palette::PaletteBrushManager::GetSingletonInstance().MakeBrush();
        PlayableComponent::PostMapLoadAll();
        UnitPool::SingleInstance().Reset();
#endif
    }

    void Application::ImGuiUpdate()
    {
#ifdef EDITOR
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

        ImGuizmo::SetOrthographic(false);
        ImGuizmo::BeginFrame();

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        window_flags |= ImGuiWindowFlags_MenuBar;
        window_flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

        {
            ImGui::SetNextWindowSize(dockspaceArea);
            ImGui::SetNextWindowPos(dockspaceStartPoint);

            ImGui::Begin("DockSpace", nullptr, window_flags);

            // Dockspace
            ImGui::DockSpace(ImGui::GetID("MyDockspace"));

            layers[(int)LayerList::EditorLayer]->Update(1);
            layers[(int)LayerList::EditorLayer]->GUIProgress();

            ImGui::End();
        }

        ImGui::Render();

        ImVec4 clear_color = ImColor(IM_COL32_WHITE);
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };

        g_pD3dDeviceContext->OMSetRenderTargets(1, &g_EditormainRenderTargetView, NULL);
        g_pD3dDeviceContext->ClearRenderTargetView(g_EditormainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        //g_EditorpSwapChain->Present(1, 0); // Present with vsync
        g_EditorpSwapChain->Present(0, 0); // Present without vsync

        // 커맨드들 실행
        cm.ExecuteCommands();

        // 이벤트들 실행
        ProcessEvents();

        layers[(int)LayerList::ContentsLayer]->Update(1);
        layers[(int)LayerList::ContentsLayer]->GUIProgress();
#endif
    }

    void Application::OnEvent(editor::EditorEvents& event)
    {
#ifdef EDITOR
        editor::EventDispatcher dispatcher(event);
        //dispatcher.Dispatch<editor::WindowResizeEvent>([this](editor::WindowResizeEvent& e) { std::cout << e.GetDebugString(); return true; });
        //dispatcher.Dispatch<editor::SaveEvent>([this](editor::SaveEvent& e) { std::cout << e.GetDebugString(); return true; });
        //dispatcher.Dispatch<editor::LoadEvent>([this](editor::LoadEvent& e) { std::cout << e.GetDebugString(); return true; });
        //dispatcher.Dispatch<editor::KeyPressedEvent>([this](editor::KeyPressedEvent& e) { std::cout << e.GetDebugString(); return true; });
        //dispatcher.Dispatch<editor::KeyDownEvent>([this](editor::KeyDownEvent& e) { std::cout << e.GetDebugString(); return true; });
        //dispatcher.Dispatch<editor::KeyReleasedEvent>([this](editor::KeyReleasedEvent& e) { std::cout << e.GetDebugString(); return true; });
        //dispatcher.Dispatch<editor::MouseButtonPressedEvent>([this](editor::MouseButtonPressedEvent& e) { std::cout << e.GetDebugString(); return true; });
        //dispatcher.Dispatch<editor::MouseButtonDownEvent>([this](editor::MouseButtonDownEvent& e) { std::cout << e.GetDebugString(); return true; });
        //dispatcher.Dispatch<editor::MouseButtonUpEvent>([this](editor::MouseButtonUpEvent& e) { std::cout << e.GetDebugString(); return true; });
        //dispatcher.Dispatch<editor::MouseMoveEvent>([this](editor::MouseMoveEvent& e) { std::cout << e.GetDebugString(); return true; });
        //dispatcher.Dispatch<editor::MouseWheelEvent>([this](editor::MouseWheelEvent& e) { std::cout << e.GetDebugString(); return true; });

        layers[(int)LayerList::EditorLayer]->OnEvent(event);
#endif
    }

    void Application::ProcessEvents()
    {
#ifdef EDITOR
        eim.Clear();
        eim.Update();

        while (em.Size() != 0)
        {
            auto eventFunc = em.PopEventCallable();
            if (eventFunc)
            {
                eventFunc();
            }
        }
#endif
    }

    void Application::SetWindowCallBack()
    {
#ifdef EDITOR
        // mouse device setting
        inputDevice[0].usUsagePage = 0x01;
        inputDevice[0].usUsage = 0x02;
        inputDevice[0].dwFlags = RIDEV_INPUTSINK;
        inputDevice[0].hwndTarget = editorHWND;

        // keyboard device setting
        inputDevice[1].usUsagePage = 0x01;
        inputDevice[1].usUsage = 0x06;
        inputDevice[1].dwFlags = RIDEV_INPUTSINK;
        inputDevice[1].hwndTarget = editorHWND;

        RegisterRawInputDevices(inputDevice, 2, sizeof(RAWINPUTDEVICE));
        gameWindowFocusCallBackFunction = [&]() { editor::EditorCamera::GetSingletonInstance().SetInputUpdate(false); };
        gameWindowKillFocusCallBackFunction = [&]() { editor::EditorCamera::GetSingletonInstance().SetInputUpdate(true); };
        winResizeCallBackFunction = [&]() { Application::DispatchEvent<editor::WindowResizeEvent>(g_EditorResizeWidth, g_EditorResizeHeight); };
        winKeyboardPressedCallBackFunction = [&](unsigned char keyCode)
            {
                if (eim.IsKeyboardDown(static_cast<editor::KeyCode>(keyCode)))
                {
                    Application::DispatchEvent<editor::KeyDownEvent>(static_cast<editor::KeyCode>(keyCode));
                }
                else
                {
                    Application::DispatchEvent<editor::KeyPressedEvent>(static_cast<editor::KeyCode>(keyCode));
                }
            };
        winKeyboardUpCallBackFunction = [&](unsigned char keyCode) { Application::DispatchEvent<editor::KeyReleasedEvent>(static_cast<editor::KeyCode>(keyCode)); };
        winMouseLeftPressedCallBackFunction = [&]()
            {
                Application::DispatchEvent<editor::MouseButtonPressedEvent>(editor::MouseCode::Left);
                if (gameFocus)
                {
                    if (IsCursorInGameWindow() == true)
                    {
                        auto front = yunutyEngine::graphics::Camera::GetMainCamera()->GetTransform()->GetWorldRotation().Forward();
                        auto distToXZPlane = abs(yunutyEngine::graphics::Camera::GetMainCamera()->GetTransform()->GetWorldPosition().y);
                        auto centeredPosition = Input::getMouseScreenPositionNormalized();
                        centeredPosition.x -= 0.5;
                        centeredPosition.y -= 0.5;
                        centeredPosition.y *= -1;
                        auto projectedPos = yunutyEngine::graphics::Camera::GetMainCamera()->GetProjectedPoint(centeredPosition, distToXZPlane, Vector3d(0, 1, 0));
                        if (Vector3d::Dot(projectedPos - yunutyEngine::graphics::Camera::GetMainCamera()->GetTransform()->GetWorldPosition(), front) > 0)
                        {
                            editor::palette::PaletteManager::GetSingletonInstance().GetCurrentPalette()->OnLeftClick();
                        }
                    }
                }
            };
        winMouseLeftUpCallBackFunction = [&]()
            {
                Application::DispatchEvent<editor::MouseButtonUpEvent>(editor::MouseCode::Left);
                if (gameFocus)
                {
                    if (IsCursorInGameWindow() == true)
                    {
                        editor::palette::PaletteManager::GetSingletonInstance().GetCurrentPalette()->OnLeftClickRelease();
                    }
                }
            };
        winMouseRightPressedCallBackFunction = [&]() { Application::DispatchEvent<editor::MouseButtonPressedEvent>(editor::MouseCode::Right); };
        winMouseRightUpCallBackFunction = [&]() { Application::DispatchEvent<editor::MouseButtonUpEvent>(editor::MouseCode::Right); };
        winMouseMoveCallBackFunction = [&](long posX, long posY)
            {
                Application::DispatchEvent<editor::MouseMoveEvent>(posX, posY);
                if (gameFocus)
                {
                    static float accumTime = Time::GetTimeElapsedUnscaled();
                    if (Time::GetTimeElapsedUnscaled() - accumTime < 0.000001)
                        return;
                    accumTime = Time::GetTimeElapsedUnscaled();

                    if (IsCursorInGameWindow() == false)
                    {
                        editor::palette::PaletteManager::GetSingletonInstance().GetCurrentPalette()->OnLeftClickRelease();
                        return;
                    }

                    auto front = yunutyEngine::graphics::Camera::GetMainCamera()->GetTransform()->GetWorldRotation().Forward();
                    auto distToXZPlane = abs(yunutyEngine::graphics::Camera::GetMainCamera()->GetTransform()->GetWorldPosition().y);
                    auto centeredPosition = Input::getMouseScreenPositionNormalized();
                    centeredPosition.x -= 0.5;
                    centeredPosition.y -= 0.5;
                    centeredPosition.y *= -1;
                    auto projectedPos = yunutyEngine::graphics::Camera::GetMainCamera()->GetProjectedPoint(centeredPosition, distToXZPlane, Vector3d(0, 1, 0));
                    editor::palette::PaletteManager::GetSingletonInstance().GetCurrentPalette()->OnMouseMove(projectedPos, centeredPosition);
                }
            };
        winMouseWheelCallBackFunction = [&](short wheelDelta) {Application::DispatchEvent<editor::MouseWheelEvent>(wheelDelta); };
#endif
    }

    void Application::CheckContentsLayerInit()
    {
#ifdef EDITOR
        auto scene = yunutyEngine::Scene::getCurrentScene();
        if (scene == nullptr)
        {
            yunutyEngine::Scene::LoadScene(new yunutyEngine::Scene());
        }
#endif
    }

    bool Application::IsCursorInGameWindow()
    {
        POINT cursorPos;
        RECT winRect;
        GetCursorPos(&cursorPos);
        GetWindowRect(hWND, &winRect);
        return (cursorPos.x >= winRect.left && cursorPos.x <= winRect.right) && (cursorPos.y >= winRect.top && cursorPos.y <= winRect.bottom);
    }

    Layer* Application::GetContentsLayer()
    {
        return layers[(int)LayerList::ContentsLayer];
    }

}

#ifdef EDITOR
// Helper functions
void GetDeviceAndDeviceContext()
{
    g_pD3dDevice = reinterpret_cast<ID3D11Device*>(yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager()->GetDevice());
    g_pD3dDeviceContext = reinterpret_cast<ID3D11DeviceContext*>(yunutyEngine::graphics::Renderer::SingleInstance().GetResourceManager()->GetDeviceContext());
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_EditorpSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pD3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_EditormainRenderTargetView);
    pBackBuffer->Release();
}

bool CreateSwapChain()
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
    auto result1 = g_pD3dDevice->QueryInterface(IID_PPV_ARGS(&dxgiDevice));

    IDXGIAdapter* dxgiAdapter;
    auto result2 = dxgiDevice->GetAdapter(&dxgiAdapter);

    IDXGIFactory2* dxgiFactory = nullptr;
    auto result3 = dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));
    dxgiAdapter->Release();

    auto result4 = dxgiFactory->CreateSwapChainForHwnd(g_pD3dDevice, editorHWND, &sd, nullptr, nullptr, &g_EditorpSwapChain);
    dxgiFactory->Release();

    CreateRenderTarget();

    return true;
}

void CleanupRenderTarget()
{
    if (g_EditormainRenderTargetView) { g_EditormainRenderTargetView->Release(); g_EditormainRenderTargetView = nullptr; }
}

void CleanupSwapChain()
{
    CleanupRenderTarget();
    if (g_EditorpSwapChain) { g_EditorpSwapChain->Release(); g_EditorpSwapChain = nullptr; }
}

void ResizeBuffers()
{
    g_EditormainRenderTargetView->Release();
    g_EditorpSwapChain->ResizeBuffers(0, dockspaceArea.x, dockspaceArea.y, DXGI_FORMAT_UNKNOWN, 0);
    CreateRenderTarget();
}
#endif

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
    switch (msg)
    {
    case WM_KILLFOCUS:
    {
        gameFocus = false;
        ShowCursor(true);
#ifdef EDITOR
        if (gameWindowKillFocusCallBackFunction)
        {
            gameWindowKillFocusCallBackFunction();
        }
#endif
        break;
    }
    case WM_SETFOCUS:
    {
        gameFocus = true;
#ifndef GRAPHICS_TEST
        ShowCursor(false);
#endif
#ifdef EDITOR
        if (gameWindowFocusCallBackFunction)
        {
            gameWindowFocusCallBackFunction();
        }
#endif
        break;
    }
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
        g_ResizeHeight = (UINT)HIWORD(lParam);
        if (wParam == SIZE_MAXIMIZED)
        {
            graphics::Renderer::SingleInstance().SetResolution(g_ResizeWidth, g_ResizeHeight);
        }
        break;
    case WM_EXITSIZEMOVE:
        graphics::Renderer::SingleInstance().SetResolution(g_ResizeWidth, g_ResizeHeight);
        break;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

#ifdef EDITOR
LRESULT WINAPI WndEditorProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (wanderUtils::ResourceRecursiveLoader::IsLoadingResources())
    {
        return true;
    }
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
    {
        return true;
    }

    static auto& eim = application::editor::EditorInputManager::GetSingletonInstance();

    switch (msg)
    {
    case WM_INPUT:
    {
        static RAWINPUT rawInput = RAWINPUT();
        UINT unSize = sizeof(RAWINPUT);
        GetRawInputData((HRAWINPUT)lParam, RID_INPUT, &rawInput, &unSize, sizeof(RAWINPUTHEADER));

        if (rawInput.header.dwType == RIM_TYPEKEYBOARD)
        {
            switch (rawInput.data.keyboard.Message)
            {
            case WM_KEYDOWN:
            {
                if (winKeyboardPressedCallBackFunction)
                {
                    eim.Update();
                    unsigned char keyCode = static_cast<unsigned char>(application::editor::EditorInputManager::GetKeyCode(rawInput.data.keyboard.VKey));
                    if (eim.IsKeyboardDown(static_cast<application::editor::KeyCode>(keyCode)))
                    {
                        eim.UpdateKeyboardState(static_cast<application::editor::KeyCode>(keyCode), application::editor::KeyState::Down);
                    }
                    else
                    {
                        eim.UpdateKeyboardState(static_cast<application::editor::KeyCode>(keyCode), application::editor::KeyState::Pressed);
                    }
                    winKeyboardPressedCallBackFunction(keyCode);
                }
                return 0;
            }
            case WM_KEYUP:
            {
                if (winKeyboardUpCallBackFunction)
                {
                    eim.Update();
                    unsigned char keyCode = static_cast<unsigned char>(application::editor::EditorInputManager::GetKeyCode(rawInput.data.keyboard.VKey));
                    eim.UpdateKeyboardState(static_cast<application::editor::KeyCode>(keyCode), application::editor::KeyState::Up);
                    winKeyboardUpCallBackFunction(keyCode);
                }
                return 0;
            }
            }
        }
        else if (rawInput.header.dwType == RIM_TYPEMOUSE)
        {
            if (rawInput.data.mouse.usFlags == MOUSE_MOVE_RELATIVE || rawInput.data.mouse.usFlags == MOUSE_MOVE_ABSOLUTE)
            {
                if (winMouseMoveCallBackFunction)
                {
                    eim.Update();
                    static POINT cursorPos;
                    GetCursorPos(&cursorPos);
                    winMouseMoveCallBackFunction(cursorPos.x, cursorPos.y);
                }
            }

            switch (rawInput.data.mouse.usButtonFlags)
            {
            case RI_MOUSE_BUTTON_1_DOWN:
            {
                if (winMouseLeftPressedCallBackFunction)
                {
                    eim.Update();
                    eim.UpdateMouseButtonState(application::editor::MouseCode::Left, application::editor::KeyState::Pressed);
                    winMouseLeftPressedCallBackFunction();
                }
                return 0;
            }
            case RI_MOUSE_BUTTON_1_UP:
            {
                if (winMouseLeftUpCallBackFunction)
                {
                    eim.Update();
                    eim.UpdateMouseButtonState(application::editor::MouseCode::Left, application::editor::KeyState::Up);
                    winMouseLeftUpCallBackFunction();
                }
                return 0;
            }
            case RI_MOUSE_BUTTON_2_DOWN:
            {
                if (winMouseRightPressedCallBackFunction)
                {
                    eim.Update();
                    eim.UpdateMouseButtonState(application::editor::MouseCode::Right, application::editor::KeyState::Pressed);
                    winMouseRightPressedCallBackFunction();
                }
                return 0;
            }
            case RI_MOUSE_BUTTON_2_UP:
            {
                if (winMouseRightUpCallBackFunction)
                {
                    eim.Update();
                    eim.UpdateMouseButtonState(application::editor::MouseCode::Right, application::editor::KeyState::Up);
                    winMouseRightUpCallBackFunction();
                }
                return 0;
            }
            case RI_MOUSE_WHEEL:
            {
                if (winMouseWheelCallBackFunction)
                {
                    eim.Update();
                    winMouseWheelCallBackFunction(rawInput.data.mouse.usButtonData);
                }
                return 0;
            }
            }
        }
        break;
    }
    case WM_SIZE:
    {
        if (wParam == SIZE_MINIMIZED)
            return 0;

        RECT rect = RECT();
        GetClientRect(hWnd, &rect);
        dockspaceArea = ImVec2(rect.right - rect.left, rect.bottom - rect.top);
        g_EditorResizeWidth = dockspaceArea.x;
        g_EditorResizeHeight = dockspaceArea.y;

        POINT windowPos = POINT();
        ClientToScreen(hWnd, &windowPos);
        dockspaceStartPoint = ImVec2(windowPos.x, windowPos.y);

        ResizeBuffers();

        if (winResizeCallBackFunction)
        {
            winResizeCallBackFunction();
        }

        return 0;
    }
    case WM_MOVE:
    {
        POINT windowPos = POINT();
        ClientToScreen(hWnd, &windowPos);
        dockspaceStartPoint = ImVec2(windowPos.x, windowPos.y);

        return 0;
    }
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
#endif


