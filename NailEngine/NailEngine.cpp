#include "NailEngine.h"

#include <assert.h>

#include "ResourceBuilder.h"
#include "Device.h"
#include "SwapChain.h"
#include "ConstantBuffer.h"

#include "Struct.h"
#include "ResourceManager.h"

#include "RenderSystem.h"

LazyObjects<NailEngine> NailEngine::Instance;

void NailEngine::Init(UINT64 hWnd)
{
	this->hWnd = reinterpret_cast<HWND>(hWnd);

	this->device = std::make_shared<Device>();
	this->device->Init();
	ResourceBuilder::Instance.Get().device = this->device;

	this->swapChain = std::make_shared<SwapChain>();
	this->swapChain->Init(this->hWnd, this->width, this->height);
	ResourceBuilder::Instance.Get().swapChain = this->swapChain;

	ResourceManager::Instance.Get().CreateDefaultResource();
	CreateConstantBuffer();
}

void NailEngine::Render()
{
	
	// Begin
	ResourceBuilder::Instance.Get().device->GetDeviceContext().Get()->RSSetViewports(1, &ResourceBuilder::Instance.Get().swapChain->GetViewPort());

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};

	depthStencilDesc.DepthEnable = true; // 깊이 버퍼 사용 여부
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; // 깊이 값을 쓰는지 여부
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS; // 깊이 값을 비교하는 함수

	depthStencilDesc.StencilEnable = false; // 스텐실 버퍼 사용 여부
	depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK; // 스텐실 값을 읽을 때의 마스크
	depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK; // 스텐실 값을 쓸 때의 마스크

	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP; // 스텐실 테스트에 실패하면 수행할 동작
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP; // 스텐실 테스트는 성공했으나 깊이 테스트는 실패한 경우에 수행할 동작
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP; // 스텐실 테스트와 깊이 테스트 모두 성공한 경우에 수행할 동작
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS; // 스텐실 테스트에 사용할 함수

	depthStencilDesc.BackFace = depthStencilDesc.FrontFace; // 양면 스텐실을 

	ID3D11DepthStencilState* pDepthStencilState;
	HRESULT hr = ResourceBuilder::Instance.Get().device->GetDevice()->CreateDepthStencilState(&depthStencilDesc, &pDepthStencilState);

	ResourceBuilder::Instance.Get().device->GetDeviceContext().Get()->OMSetDepthStencilState(pDepthStencilState, 1);

	const float red[] = { 1.f, 0.f, 0.f, 1.f };

	// 렌더 타겟 뷰를 바인딩하고, 뎁스 스텐실 버퍼를 출력 병합기 단계에 바인딩한다.
	ResourceBuilder::Instance.Get().device->GetDeviceContext().Get()->OMSetRenderTargets(1,
		ResourceBuilder::Instance.Get().swapChain->GetRTV().GetAddressOf(),
		ResourceBuilder::Instance.Get().swapChain->GetDSV().Get());

	// 렌더 타겟뷰를 내가 지정한 값으로 픽셀을 다 초기화하여 지운다.
	ResourceBuilder::Instance.Get().device->GetDeviceContext().Get()->ClearRenderTargetView(ResourceBuilder::Instance.Get().swapChain->GetRTV().Get(), red);
	// 뎁스 스텐실뷰를 초기화한다.
	ResourceBuilder::Instance.Get().device->GetDeviceContext().Get()->ClearDepthStencilView(ResourceBuilder::Instance.Get().swapChain->GetDSV().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	ResourceBuilder::Instance.Get().device->GetDeviceContext().Get()->RSSetState(0);

	RenderSystem::Instance.Get().RenderObject();

	// End
	ResourceBuilder::Instance.Get().swapChain->GetSwapChain().Get()->Present(1, 0);

	for (auto& iter : this->constantBuffers)
	{
		iter->Clear();
	}
}

void NailEngine::SetResolution(unsigned int width, unsigned int height)
{
	this->width = width;
	this->height = height;
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
}
