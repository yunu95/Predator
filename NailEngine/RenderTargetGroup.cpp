#include "RenderTargetGroup.h"

#include "ResourceBuilder.h"
#include "Device.h"
#include "SwapChain.h"
#include "Texture.h"

RenderTargetGroup::~RenderTargetGroup()
{
}

void RenderTargetGroup::OMSetRenderTarget(bool isPBR)
{
	if (!isPBR)
	{
		ResourceBuilder::Instance.Get().device->GetDeviceContext().Get()->RSSetViewports(1, &ResourceBuilder::Instance.Get().swapChain->GetViewPort());
		ResourceBuilder::Instance.Get().device->GetDeviceContext()->OMSetRenderTargets(this->rtCount, RTVVec.data(), ResourceBuilder::Instance.Get().swapChain->GetDSV().Get());
	}
	else
	{
		ResourceBuilder::Instance.Get().device->GetDeviceContext().Get()->RSSetViewports(1, &ResourceBuilder::Instance.Get().swapChain->GetViewPort());
		ResourceBuilder::Instance.Get().device->GetDeviceContext()->OMSetRenderTargets(this->rtCount, RTVVec.data(), nullptr);
	}


	//if (this->dsv == nullptr)
	//{
	//	ResourceBuilder::Instance.Get().device->GetDeviceContext().Get()->RSSetViewports(1, &ResourceBuilder::Instance.Get().swapChain->GetViewPort());
	//	ResourceBuilder::Instance.Get().device->GetDeviceContext()->OMSetRenderTargets(this->rtCount, RTVVec.data(), ResourceBuilder::Instance.Get().swapChain->GetDSV().Get());
	//}
	//else
	//{
	//	D3D11_VIEWPORT viewport;
	//	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	//	viewport.TopLeftX = 0; // Viewport의 왼쪽 상단 X 좌표
	//	viewport.TopLeftY = 0; // Viewport의 왼쪽 상단 Y 좌표
	//	viewport.Width = SM_SIZE; // Viewport의 너비
	//	viewport.Height = SM_SIZE; // Viewport의 높이
	//	viewport.MinDepth = 0.0f; // Viewport의 최소 깊이 값
	//	viewport.MaxDepth = 1.0f; // Viewport의 최대 깊이 값

	//	// 디바이스 컨텍스트에 Viewport 설정 적용
	//	ResourceBuilder::Instance.Get().device->GetDeviceContext().Get()->RSSetViewports(1, &viewport);
	//	ResourceBuilder::Instance.Get().device->GetDeviceContext()->OMSetRenderTargets(this->rtCount, RTVVec.data(), this->dsv.Get());
	//}
}

void RenderTargetGroup::UnBind()
{
	ResourceBuilder::Instance.Get().device->GetDeviceContext()->OMSetRenderTargets(this->rtCount, nullRTV.data(), ResourceBuilder::Instance.Get().swapChain->GetDSV().Get());
}

void RenderTargetGroup::Clear()
{
	for (int i = 0; i < this->rtVec.size(); ++i)
	{
		ResourceBuilder::Instance.Get().device->GetDeviceContext()->ClearRenderTargetView(this->rtVec[i].texture->GetRTV().Get(), rtVec[i].clearColor);
	}
}

void RenderTargetGroup::Release()
{
	RTVVec.clear();
	rtVec.clear();
	nullRTV.clear();
	if (dsv != nullptr)
	{
		dsv->Release();
	}
}

void RenderTargetGroup::SetRenderTargetVec(std::vector<RenderTarget>& rtVec, Microsoft::WRL::ComPtr<ID3D11DepthStencilView> dsv)
{
	this->rtVec = rtVec;
	this->rtCount = rtVec.size();

	for (int i = 0; i < this->rtCount; ++i)
	{
		this->RTVVec.emplace_back(rtVec[i].texture->GetRTV().Get());
		nullRTV.emplace_back(nullptr);
	}

	if (dsv != nullptr)
	{
		this->dsv = dsv;
	}
}

std::shared_ptr<Texture>& RenderTargetGroup::GetRTTexture(int index)
{
	return this->rtVec[index].texture;
}
