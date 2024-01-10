#include "RenderTargetGroup.h"

#include "ResourceBuilder.h"
#include "Device.h"
#include "SwapChain.h"
#include "Texture.h"

RenderTargetGroup::~RenderTargetGroup()
{
}

void RenderTargetGroup::OMSetRenderTarget()
{
	if (this->dsv == nullptr)
	{
		ResourceBuilder::Instance.Get().device->GetDeviceContext()->OMSetRenderTargets(this->rtCount, RTVVec.data(), ResourceBuilder::Instance.Get().swapChain->GetDSV().Get());
	}
	else
	{
		ResourceBuilder::Instance.Get().device->GetDeviceContext()->OMSetRenderTargets(this->rtCount, RTVVec.data(), this->dsv.Get());
	}
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
