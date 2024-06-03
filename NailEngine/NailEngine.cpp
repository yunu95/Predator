#include "NailEngine.h"

#include <assert.h>
#include <shlobj.h>
#include <filesystem>
#include <ranges>
#include <iostream>


#include "ResourceBuilder.h"
#include "Device.h"
#include "SwapChain.h"
#include "ConstantBuffer.h"

#include "Struct.h"
#include "ResourceManager.h"

#include "RenderSystem.h"
#include "RenderTargetGroup.h"

#include "InstancingManager.h"
#include "ShadowPass.h"
#include "PointLightShadowPass.h"
#include "SkyBoxPass.h"
#include "BloomPass.h"

#include "ILight.h"

#include "Texture.h"

#include "CameraManager.h"
#include "NailCamera.h"

#include <mfapi.h>
#include <mfplay.h>
#include <mfreadwrite.h>
#include <mferror.h>


LazyObjects<NailEngine> NailEngine::Instance;

void NailEngine::Init(UINT64 hWnd)
{
    this->windowInfo.hWnd = reinterpret_cast<HWND>(hWnd);

    this->device = std::make_shared<Device>();
    this->device->Init();
    ResourceBuilder::Instance.Get().device = this->device;

    this->swapChain = std::make_shared<SwapChain>();
    this->swapChain->Init(this->windowInfo.hWnd, this->windowInfo.width, this->windowInfo.height);
    ResourceBuilder::Instance.Get().swapChain = this->swapChain;

    CreateConstantBuffer();
    CreateRenderTargetGroup();
    ResourceManager::Instance.Get().CreateDefaultResource();

    RenderSystem::Instance.Get().Init();

    InstancingManager::Instance.Get().Init();

    ShadowPass::Instance.Get().Init(ResourceManager::Instance.Get().GetTexture(L"ShadowDepth").get(),
        reinterpret_cast<VertexShader*>(ResourceManager::Instance.Get().GetShader(L"TestVS.cso").get()),
        reinterpret_cast<PixelShader*>(ResourceManager::Instance.Get().GetShader(L"TestPS.cso").get()),
        reinterpret_cast<VertexShader*>(ResourceManager::Instance.Get().GetShader(L"SkinnedShadowVS.cso").get()));

    SkyBoxPass::Instance.Get().Init(
        ResourceManager::Instance.Get().GetTexture(L"Texture/asdEnvHDR.dds").get(),
        ResourceManager::Instance.Get().GetMesh(L"Sphere").get(),
        ResourceManager::Instance.Get().GetShader(L"SkyBoxVS.cso").get(),
        ResourceManager::Instance.Get().GetShader(L"SkyBoxPS.cso").get());

    BloomPass::Instance.Get().Init();

    PointLightShadowPass::Instance.Get().Init();

    this->lightMap = ResourceManager::Instance.Get().GetTexture(L"Stage1LightMap").get();
    // 비디오 출력을 위한 API 생성
    HRESULT hr = MFStartup(MF_VERSION);
}

void NailEngine::Render()
{

    // Begin
    //ResourceBuilder::Instance.Get().device->GetDeviceContext().Get()->RSSetViewports(1, &ResourceBuilder::Instance.Get().swapChain->GetViewPort());
    //float4(0.7686, 0.8784, 0.9451, 1.f)
    const float red[] = { 0.7686, 0.8784, 0.9451, 1.f };

    // 렌더 타겟뷰를 내가 지정한 값으로 픽셀을 다 초기화하여 지운다.
    ResourceBuilder::Instance.Get().device->GetDeviceContext().Get()->ClearRenderTargetView(ResourceBuilder::Instance.Get().swapChain->GetRTV().Get(), red);
    // 뎁스 스텐실뷰를 초기화한다.
    ResourceBuilder::Instance.Get().device->GetDeviceContext().Get()->ClearDepthStencilView(ResourceBuilder::Instance.Get().swapChain->GetDSV().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

    RenderSystem::Instance.Get().Render();

    // End
    ResourceBuilder::Instance.Get().swapChain->GetSwapChain().Get()->Present(0, 0);





    for (auto& iter : this->constantBuffers)
    {
        iter->Clear();
    }

    for (int i = 0; i < this->renderTargetGroup.size(); ++i)
    {
        this->renderTargetGroup[i]->UnBind();

        //if (i == static_cast<int>(RENDER_TARGET_TYPE::IRRADIANCE)) continue;

        this->renderTargetGroup[i]->Clear();
    }
}

void NailEngine::Finalize()
{
    // 비디오 출력을 위한 API 종료
    MFShutdown();
    RenderSystem::Instance.Get().Finalize();
}

void NailEngine::SetLightMap(const std::wstring& lightMapName)
{
    this->lightMap = ResourceManager::Instance.Get().GetTexture(lightMapName).get();
}

Texture* NailEngine::GetLightMap()
{
    return this->lightMap;
}

void NailEngine::SetResolution(unsigned int width, unsigned int height)
{
    this->windowInfo.width = width;
    this->windowInfo.height = height;
}

void NailEngine::SetUseIBL(bool useIBL)
{
    this->useIBL = useIBL;
}

void NailEngine::SetUseLightMap(bool useLightMap)
{
    this->useLightMap = useLightMap;
}

void NailEngine::ResizeResolution(unsigned int width, unsigned int height)
{
    this->windowInfo.width = width;
    this->windowInfo.height = height;

    // 화면의 해상도가 달라졌으니 해상도와 관련된 부분 다 지우고 다시 만들기
    // 1. 스왑체인 다시 만들기

    ID3D11RenderTargetView* nullViews[] = { nullptr };

    ResourceBuilder::Instance.Get().device->GetDeviceContext()->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);


    RenderSystem::Instance.Get().ReleaseD2D();
    ResourceBuilder::Instance.Get().swapChain = nullptr;
    this->swapChain->Release();
    this->swapChain->Resize(width, height);
    ResourceBuilder::Instance.Get().swapChain = this->swapChain;
    RenderSystem::Instance.Get().CreateD2D();



    // 2. 렌더타겟들 다시 만들기
    for (auto& each : renderTargetGroup)
    {
        each->Release();
        each.reset();
    }

    this->CreateRenderTargetGroup();

    auto& cameraList = CameraManager::Instance.Get().GetCamearaList();
    for (auto& each : cameraList)
    {
        each.second->SetResolution(width, height);
    }
}

void NailEngine::SetClearColor(yunuGI::Color color)
{
    auto& rt = this->renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::FINAL)]->GetRT(0);
    rt.clearColor[0] = color.r;
    rt.clearColor[1] = color.g;
    rt.clearColor[2] = color.b;
    rt.clearColor[3] = color.a;
}

std::shared_ptr<ConstantBuffer>& NailEngine::GetConstantBuffer(unsigned int index)
{
    return this->constantBuffers[index];
}

bool NailEngine::GetUseIBL()
{
    return this->useIBL;
}

bool NailEngine::GetUseLightMap()
{
    return this->useLightMap;
}

void NailEngine::CreateConstantBuffer()
{
    {
        std::shared_ptr<ConstantBuffer> _constantBuffer = std::make_shared<ConstantBuffer>();
        _constantBuffer->CraeteConstantBuffer(sizeof(MatrixBuffer));
        this->constantBuffers.emplace_back(_constantBuffer);
    }

    {
        std::shared_ptr<ConstantBuffer> _constantBuffer = std::make_shared<ConstantBuffer>();
        _constantBuffer->CraeteConstantBuffer(sizeof(MaterialBuffer));
        this->constantBuffers.emplace_back(_constantBuffer);
    }

    {
        std::shared_ptr<ConstantBuffer> _constantBuffer = std::make_shared<ConstantBuffer>();
        _constantBuffer->CraeteConstantBuffer(sizeof(LightParams));
        this->constantBuffers.emplace_back(_constantBuffer);
    }

    {
        std::shared_ptr<ConstantBuffer> _constantBuffer = std::make_shared<ConstantBuffer>();
        _constantBuffer->CraeteConstantBuffer(sizeof(CameraBuffer));
        this->constantBuffers.emplace_back(_constantBuffer);
    }

    {
        std::shared_ptr<ConstantBuffer> _constantBuffer = std::make_shared<ConstantBuffer>();
        _constantBuffer->CraeteConstantBuffer(sizeof(BoneMatrix));
        this->constantBuffers.emplace_back(_constantBuffer);
    }

    {
        std::shared_ptr<ConstantBuffer> _constantBuffer = std::make_shared<ConstantBuffer>();
        _constantBuffer->CraeteConstantBuffer(sizeof(InstanceTransitionDesc));
        this->constantBuffers.emplace_back(_constantBuffer);
    }

    {
        std::shared_ptr<ConstantBuffer> _constantBuffer = std::make_shared<ConstantBuffer>();
        _constantBuffer->CraeteConstantBuffer(sizeof(FogBuffer));
        this->constantBuffers.emplace_back(_constantBuffer);
    }

    {
        std::shared_ptr<ConstantBuffer> _constantBuffer = std::make_shared<ConstantBuffer>();
        _constantBuffer->CraeteConstantBuffer(sizeof(PointLightVPMatrix));
        this->constantBuffers.emplace_back(_constantBuffer);
    }

    {
        std::shared_ptr<ConstantBuffer> _constantBuffer = std::make_shared<ConstantBuffer>();
        _constantBuffer->CraeteConstantBuffer(sizeof(PointLightIndex));
        this->constantBuffers.emplace_back(_constantBuffer);
    }

    {
        std::shared_ptr<ConstantBuffer> _constantBuffer = std::make_shared<ConstantBuffer>();
        _constantBuffer->CraeteConstantBuffer(sizeof(ExposureBuffer));
        this->constantBuffers.emplace_back(_constantBuffer);
    }

    {
        std::shared_ptr<ConstantBuffer> _constantBuffer = std::make_shared<ConstantBuffer>();
        _constantBuffer->CraeteConstantBuffer(sizeof(UtilBuffer));
        this->constantBuffers.emplace_back(_constantBuffer);
    }

    {
        std::shared_ptr<ConstantBuffer> _constantBuffer = std::make_shared<ConstantBuffer>();
        _constantBuffer->CraeteConstantBuffer(sizeof(ParticleBuffer));
        this->constantBuffers.emplace_back(_constantBuffer);
    }

    {
        std::shared_ptr<ConstantBuffer> _constantBuffer = std::make_shared<ConstantBuffer>();
        _constantBuffer->CraeteConstantBuffer(sizeof(LightMapUVBuffer));
        this->constantBuffers.emplace_back(_constantBuffer);
    }
}

void NailEngine::CreateRenderTargetGroup()
{
    this->renderTargetGroup.resize(static_cast<int>(RENDER_TARGET_TYPE::END));

    // G_BUFFER
    {
        std::vector<RenderTarget> rtVec(G_BUFFER_MEMBER_COUNT);

        rtVec[0].texture = std::static_pointer_cast<Texture>(ResourceManager::Instance.Get().CreateTexture(
            L"PositionTarget",
            this->windowInfo.width,
            this->windowInfo.height,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            static_cast<D3D11_BIND_FLAG>(D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE)
        ));

        rtVec[1].texture = std::static_pointer_cast<Texture>(ResourceManager::Instance.Get().CreateTexture(
            L"NormalTarget",
            this->windowInfo.width,
            this->windowInfo.height,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            static_cast<D3D11_BIND_FLAG>(D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE)
        ));

        rtVec[2].texture = std::static_pointer_cast<Texture>(ResourceManager::Instance.Get().CreateTexture(
            L"AlbedoTarget",
            this->windowInfo.width,
            this->windowInfo.height,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            static_cast<D3D11_BIND_FLAG>(D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE)
        ));

        rtVec[3].texture = std::static_pointer_cast<Texture>(ResourceManager::Instance.Get().CreateTexture(
            L"UtilTarget",
            this->windowInfo.width,
            this->windowInfo.height,
            DXGI_FORMAT_R16G16B16A16_FLOAT,
            static_cast<D3D11_BIND_FLAG>(D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE)
        ));

        rtVec[4].texture = std::static_pointer_cast<Texture>(ResourceManager::Instance.Get().CreateTexture(
            L"ARMTarget",
            this->windowInfo.width,
            this->windowInfo.height,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            static_cast<D3D11_BIND_FLAG>(D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE)
        ));

        rtVec[5].texture = std::static_pointer_cast<Texture>(ResourceManager::Instance.Get().CreateTexture(
            L"EmissiveTarget",
            this->windowInfo.width,
            this->windowInfo.height,
            DXGI_FORMAT_R8G8B8A8_UNORM,
            static_cast<D3D11_BIND_FLAG>(D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE)
        ));

        this->renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::G_BUFFER)] = std::make_shared<RenderTargetGroup>();
        this->renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::G_BUFFER)]->SetRenderTargetVec(rtVec);
    }

    // LIGHTING
    {
        std::vector<RenderTarget> rtVec(LIGHTING_MEMBER_COUNT);

        rtVec[0].texture = std::static_pointer_cast<Texture>(ResourceManager::Instance.Get().CreateTexture(
            L"DiffuseLightTarget",
            this->windowInfo.width,
            this->windowInfo.height,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            static_cast<D3D11_BIND_FLAG>(D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE)
        ));

        rtVec[1].texture = std::static_pointer_cast<Texture>(ResourceManager::Instance.Get().CreateTexture(
            L"AmbientLightTarget",
            this->windowInfo.width,
            this->windowInfo.height,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            static_cast<D3D11_BIND_FLAG>(D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE)
        ));

        rtVec[2].texture = std::static_pointer_cast<Texture>(ResourceManager::Instance.Get().CreateTexture(
            L"LightShadowTarget",
            this->windowInfo.width,
            this->windowInfo.height,
            DXGI_FORMAT_R8G8B8A8_UNORM,
            static_cast<D3D11_BIND_FLAG>(D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE)
        ));

        this->renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::LIGHTING)] = std::make_shared<RenderTargetGroup>();
        this->renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::LIGHTING)]->SetRenderTargetVec(rtVec);
    }

    /// PBR
    {
        // Irradiance Map
        std::vector<RenderTarget> rtVec(IRRADIANCE_MEMBER_COUNT);
        rtVec[0].texture = std::static_pointer_cast<Texture>(ResourceManager::Instance.Get().CreateTexture(
            L"IrradianceTarget",
            32,
            32,
            DXGI_FORMAT_R16G16B16A16_FLOAT,
            static_cast<D3D11_BIND_FLAG>(D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE)
        ));
        this->renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::IRRADIANCE)] = std::make_shared<RenderTargetGroup>();
        this->renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::IRRADIANCE)]->SetRenderTargetVec(rtVec);
    }

    {
        // Specular IBL Map
        std::vector<RenderTarget> rtVec(SPECIBL_MEMBER_COUNT);
        rtVec[0].texture = std::static_pointer_cast<Texture>(ResourceManager::Instance.Get().CreateTexture(
            L"SpecularIBLTarget",
            512,
            512,
            DXGI_FORMAT_R16G16B16A16_FLOAT,
            static_cast<D3D11_BIND_FLAG>(D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE)
        ));
        this->renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::SPECLIBL)] = std::make_shared<RenderTargetGroup>();
        this->renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::SPECLIBL)]->SetRenderTargetVec(rtVec);
    }
    // Final
    {
        std::vector<RenderTarget> rtVec(FINAL_COUNT);
        rtVec[0].texture = std::static_pointer_cast<Texture>(ResourceManager::Instance.Get().CreateTexture(
            L"FinalTarget",
            this->windowInfo.width,
            this->windowInfo.height,
            DXGI_FORMAT_R16G16B16A16_FLOAT,
            static_cast<D3D11_BIND_FLAG>(D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE)
        ));

		rtVec[0].clearColor[0] = 0.7686;
		rtVec[0].clearColor[1] = 0.8784;
		rtVec[0].clearColor[2] = 0.9451;
		rtVec[0].clearColor[3] = 1.f;

        this->renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::FINAL)] = std::make_shared<RenderTargetGroup>();
        this->renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::FINAL)]->SetRenderTargetVec(rtVec);
    }

    // Bloom 전용 렌더타겟
    {
        {
            std::vector<RenderTarget> rtVec(DOWN2X2_0_COUNT);
            rtVec[0].texture = std::static_pointer_cast<Texture>(ResourceManager::Instance.Get().CreateTexture(
                L"DownSample2x2_0_Target",
                this->windowInfo.width / 2,
                this->windowInfo.height / 2,
                DXGI_FORMAT_R8G8B8A8_UNORM,
                static_cast<D3D11_BIND_FLAG>(D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE)
            ));
            this->renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::DOWN2X2_0)] = std::make_shared<RenderTargetGroup>();
            this->renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::DOWN2X2_0)]->SetRenderTargetVec(rtVec);
        }

        {
            std::vector<RenderTarget> rtVec(DOWN2X2_1_COUNT);
            rtVec[0].texture = std::static_pointer_cast<Texture>(ResourceManager::Instance.Get().CreateTexture(
                L"DownSample2x2_1_Target",
                (this->windowInfo.width / 2) / 2,
                (this->windowInfo.height / 2) / 2,
                DXGI_FORMAT_R8G8B8A8_UNORM,
                static_cast<D3D11_BIND_FLAG>(D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE)
            ));
            this->renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::DOWN2X2_1)] = std::make_shared<RenderTargetGroup>();
            this->renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::DOWN2X2_1)]->SetRenderTargetVec(rtVec);
        }

        {
            std::vector<RenderTarget> rtVec(DOWN2X2_2_COUNT);
            rtVec[0].texture = std::static_pointer_cast<Texture>(ResourceManager::Instance.Get().CreateTexture(
                L"DownSample2x2_2_Target",
                ((this->windowInfo.width / 2) / 2) / 2,
                ((this->windowInfo.height / 2) / 2) / 2,
                DXGI_FORMAT_R8G8B8A8_UNORM,
                static_cast<D3D11_BIND_FLAG>(D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE)
            ));
            this->renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::DOWN2X2_2)] = std::make_shared<RenderTargetGroup>();
            this->renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::DOWN2X2_2)]->SetRenderTargetVec(rtVec);
        }

        {
            std::vector<RenderTarget> rtVec(BLUR_COUNT);
            rtVec[0].texture = std::static_pointer_cast<Texture>(ResourceManager::Instance.Get().CreateTexture(
                L"Blur_Target",
                ((this->windowInfo.width / 2) / 2) / 2,
                ((this->windowInfo.height / 2) / 2) / 2,
                //this->windowInfo.width,
                //this->windowInfo.height,
                DXGI_FORMAT_R8G8B8A8_UNORM,
                static_cast<D3D11_BIND_FLAG>(D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE)
            ));
            this->renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::BLUR)] = std::make_shared<RenderTargetGroup>();
            this->renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::BLUR)]->SetRenderTargetVec(rtVec);
        }

        {
            std::vector<RenderTarget> rtVec(UP2X2_0_COUNT);
            rtVec[0].texture = std::static_pointer_cast<Texture>(ResourceManager::Instance.Get().CreateTexture(
                L"Up2x2_0_Target",
                ((this->windowInfo.width / 2) / 2),
                ((this->windowInfo.height / 2) / 2),
                DXGI_FORMAT_R8G8B8A8_UNORM,
                static_cast<D3D11_BIND_FLAG>(D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE)
            ));
            this->renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::UP2X2_0)] = std::make_shared<RenderTargetGroup>();
            this->renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::UP2X2_0)]->SetRenderTargetVec(rtVec);
        }

        {
            std::vector<RenderTarget> rtVec(UP2X2_1_COUNT);
            rtVec[0].texture = std::static_pointer_cast<Texture>(ResourceManager::Instance.Get().CreateTexture(
                L"Up2x2_1_Target",
                ((this->windowInfo.width / 2)),
                ((this->windowInfo.height / 2)),
                DXGI_FORMAT_R8G8B8A8_UNORM,
                static_cast<D3D11_BIND_FLAG>(D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE)
            ));
            this->renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::UP2X2_1)] = std::make_shared<RenderTargetGroup>();
            this->renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::UP2X2_1)]->SetRenderTargetVec(rtVec);
        }

        {
            std::vector<RenderTarget> rtVec(UP2X2_2_COUNT);
            rtVec[0].texture = std::static_pointer_cast<Texture>(ResourceManager::Instance.Get().CreateTexture(
                L"Up2x2_2_Target",
                ((this->windowInfo.width)),
                ((this->windowInfo.height)),
                DXGI_FORMAT_R8G8B8A8_UNORM,
                static_cast<D3D11_BIND_FLAG>(D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE)
            ));
            this->renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::UP2X2_2)] = std::make_shared<RenderTargetGroup>();
            this->renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::UP2X2_2)]->SetRenderTargetVec(rtVec);
        }
    }

    {
        std::vector<RenderTarget> rtVec(1);
        rtVec[0].texture = std::static_pointer_cast<Texture>(ResourceManager::Instance.Get().CreateTexture(
            L"TempRTV",
            SM_SIZE,
            SM_SIZE,
            //this->windowInfo.width,
            //this->windowInfo.height,
            DXGI_FORMAT_R8G8B8A8_UNORM,
            static_cast<D3D11_BIND_FLAG>(D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE)
        ));
        ShadowPass::Instance.Get().SetTempRTV(rtVec[0].texture.get());
    }
}