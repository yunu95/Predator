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

#include "ILight.h"


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
}

void NailEngine::Render()
{
	// Begin
	ResourceBuilder::Instance.Get().device->GetDeviceContext().Get()->RSSetViewports(1, &ResourceBuilder::Instance.Get().swapChain->GetViewPort());

	const float red[] = { 0.5f, 0.5f, 0.5f, 1.f };

	// 렌더 타겟뷰를 내가 지정한 값으로 픽셀을 다 초기화하여 지운다.
	ResourceBuilder::Instance.Get().device->GetDeviceContext().Get()->ClearRenderTargetView(ResourceBuilder::Instance.Get().swapChain->GetRTV().Get(), red);
	// 뎁스 스텐실뷰를 초기화한다.
	ResourceBuilder::Instance.Get().device->GetDeviceContext().Get()->ClearDepthStencilView(ResourceBuilder::Instance.Get().swapChain->GetDSV().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	RenderSystem::Instance.Get().Render();

	// End
	ResourceBuilder::Instance.Get().swapChain->GetSwapChain().Get()->Present(1, 0);

	for (auto& iter : this->constantBuffers)
	{
		iter->Clear();
	}

	for (auto& iter : this->renderTargetGroup)
	{
		iter->Clear();
	}

	for (auto& iter : this->renderTargetGroup)
	{
		iter->UnBind();
	}
}

void NailEngine::SetResolution(unsigned int width, unsigned int height)
{
	this->windowInfo.width = width;
	this->windowInfo.height = height;
}

std::shared_ptr<ConstantBuffer>& NailEngine::GetConstantBuffer(unsigned int index)
{
	return this->constantBuffers[index];
}

void NailEngine::CreateConstantBuffer()
{
	{
		std::shared_ptr<ConstantBuffer> _constantBuffer = std::make_shared<ConstantBuffer>();
		_constantBuffer->CraeteConstantBuffer(sizeof(MatrixBuffer), 256);
		this->constantBuffers.emplace_back(_constantBuffer);
	}

	{
		std::shared_ptr<ConstantBuffer> _constantBuffer = std::make_shared<ConstantBuffer>();
		_constantBuffer->CraeteConstantBuffer(sizeof(MaterialBuffer), 256);
		this->constantBuffers.emplace_back(_constantBuffer);
	}

	{
		std::shared_ptr<ConstantBuffer> _constantBuffer = std::make_shared<ConstantBuffer>();
		_constantBuffer->CraeteConstantBuffer(sizeof(LightParams), 1);
		this->constantBuffers.emplace_back(_constantBuffer);
	}

	{
		std::shared_ptr<ConstantBuffer> _constantBuffer = std::make_shared<ConstantBuffer>();
		_constantBuffer->CraeteConstantBuffer(sizeof(CameraBuffer), 256);
		this->constantBuffers.emplace_back(_constantBuffer);
	}

	{
		std::shared_ptr<ConstantBuffer> _constantBuffer = std::make_shared<ConstantBuffer>();
		_constantBuffer->CraeteConstantBuffer(sizeof(BoneMatrix), 256);
		this->constantBuffers.emplace_back(_constantBuffer);
	}
}

void NailEngine::CreateRenderTargetGroup()
{
	this->renderTargetGroup.resize(static_cast<int>(RENDER_TARGET_TYPE::END));
	//// SwapChain
	//{
	//	std::vector<RenderTarget> rtVec(SWAP_CHAIN_BUFFER_COUNT);
	//	for (int i = 0; i < SWAP_CHAIN_BUFFER_COUNT; i++)
	//	{
	//		std::wstring name = L"SwapChainTarget_" + std::to_wstring(i);

	//		Microsoft::WRL::ComPtr<ID3D11Texture2D> tex2D;
	//		this->swapChain->GetSwapChain()->GetBuffer(i, __uuidof(ID3D11Texture2D), &tex2D);
	//		rtVec[i].texture = ResourceManager::Instance.Get().CreateTextureFromResource(name, tex2D);
	//	}
	//	this->renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::SWAP_CHAIN)] = std::make_shared<RenderTargetGroup>();
	//	this->renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::SWAP_CHAIN)]->SetRenderTargetVec(rtVec);
	//}

	// G_BUFFER
	{
		std::vector<RenderTarget> rtVec(G_BUFFER_MEMBER_COUNT);

		rtVec[0].texture = ResourceManager::Instance.Get().CreateTexture(
			L"PositionTarget",
			this->windowInfo.width,
			this->windowInfo.height,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE
		);

		rtVec[1].texture = ResourceManager::Instance.Get().CreateTexture(
			L"NormalTarget",
			this->windowInfo.width,
			this->windowInfo.height,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE
		);

		rtVec[2].texture = ResourceManager::Instance.Get().CreateTexture(
			L"AlbedoTarget",
			this->windowInfo.width,
			this->windowInfo.height,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE
		);

		rtVec[3].texture = ResourceManager::Instance.Get().CreateTexture(
			L"DepthTarget",
			this->windowInfo.width,
			this->windowInfo.height,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE
		);

		this->renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::G_BUFFER)] = std::make_shared<RenderTargetGroup>();
		this->renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::G_BUFFER)]->SetRenderTargetVec(rtVec);
	}

	// LIGHTING
	{
		std::vector<RenderTarget> rtVec(LIGHTING_MEMBER_COUNT);

		rtVec[0].texture = ResourceManager::Instance.Get().CreateTexture(
			L"DiffuseLightTarget",
			this->windowInfo.width,
			this->windowInfo.height,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE
		);

		rtVec[1].texture = ResourceManager::Instance.Get().CreateTexture(
			L"SpecularLightTarget",
			this->windowInfo.width,
			this->windowInfo.height,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE
		);

		this->renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::LIGHTING)] = std::make_shared<RenderTargetGroup>();
		this->renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::LIGHTING)]->SetRenderTargetVec(rtVec);
	}
}
