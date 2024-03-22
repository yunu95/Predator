#include "PointLightShadowPass.h"
#include "RenderSystem.h"
#include "Struct.h"
#include "ResourceBuilder.h"
#include "ResourceManager.h"
#include "Device.h"
#include "Texture.h"
#include "CameraManager.h"
#include "NailCamera.h"
#include "NailEngine.h"
#include "ConstantBuffer.h"
#include "VertexShader.h"
#include "GeometryShader.h"
#include "PixelShader.h"

LazyObjects<PointLightShadowPass> PointLightShadowPass::Instance;

void PointLightShadowPass::Init()
{
	this->dsTexture = ResourceManager::Instance.Get().GetTexture(L"PointLightShadowDepth").get();
	this->vs = reinterpret_cast<VertexShader*>(ResourceManager::Instance.Get().GetShader(L"PointLightShadowVS.cso").get());
	this->gs = reinterpret_cast<GeometryShader*>(ResourceManager::Instance.Get().GetShader(L"PointLightShadowGS.cso").get());
	this->ps = reinterpret_cast<PixelShader*>(ResourceManager::Instance.Get().GetShader(L"PointLightShadowPS.cso").get());

	this->skinnedVS = reinterpret_cast<VertexShader*>(ResourceManager::Instance.Get().GetShader(L"Skinned_PointLightShadowVS.cso").get());
}

void PointLightShadowPass::Render(int index, bool isSkinned)
{
	// VSSet
	if (isSkinned == false)
	{
		// Null RenderTarget / TextureArray DSV Set
		float clearDepth = 1.0f;

		auto& dsvArray = this->dsTexture->GetDSVArray();

		ResourceBuilder::Instance.Get().device->GetDeviceContext()->ClearDepthStencilView(dsvArray[index].Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, clearDepth, 0);

		D3D11_VIEWPORT viewport = { 0.0f, 0.0f, static_cast<float>(PL_SM_SIZE), static_cast<float>(PL_SM_SIZE), 0.0f, 1.0f };
		ResourceBuilder::Instance.Get().device->GetDeviceContext()->RSSetViewports(1, &viewport);

		ResourceBuilder::Instance.Get().device->GetDeviceContext()->OMSetRenderTargets(0, nullptr, dsvArray[index].Get());

		vs->Bind();
	}
	else
	{
		skinnedVS->Bind();
	}

	// GSSet
	gs->Bind();

	// PSSet
	ps->Bind();

}

void PointLightShadowPass::EndRender()
{
	// VSSet
	vs->UnBind();

	// GSSet
	gs->UnBind();

	// PSSet
	ps->UnBind();

	auto& dsvArray = this->dsTexture->GetDSVArray();
	for (int i = 0; i < dsvArray.size(); ++i)
	{
		ResourceBuilder::Instance.Get().device->GetDeviceContext()->OMSetRenderTargets(0, nullptr, nullptr);
	}
}

