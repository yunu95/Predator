#include "ShadowPass.h"
#include "Struct.h"
#include "ResourceBuilder.h"
#include "Device.h"
#include "Texture.h"

LazyObjects<ShadowPass> ShadowPass::Instance;

//void ShadowPass::Init()
//{
//	D3D11_TEXTURE2D_DESC texDesc;
//	texDesc.Width = SM_SIZE;
//	texDesc.Height = SM_SIZE;
//	texDesc.MipLevels = 1;
//	texDesc.ArraySize = 1;
//	texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
//	texDesc.SampleDesc.Count = 1;
//	texDesc.SampleDesc.Quality = 0;
//	texDesc.Usage = D3D11_USAGE_DEFAULT;
//	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
//	texDesc.CPUAccessFlags = 0;
//	texDesc.MiscFlags = 0;
//
//	ResourceBuilder::Instance.Get().device->GetDevice()->CreateTexture2D(&texDesc, 0, &this->tex2D);
//
//	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
//	dsvDesc.Flags = 0;
//	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
//	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
//	dsvDesc.Texture2D.MipSlice = 0;
//	ResourceBuilder::Instance.Get().device->GetDevice()->CreateDepthStencilView(this->tex2D.Get(), &dsvDesc, this->dsv.GetAddressOf());
//
//	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
//	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
//	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
//	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
//	srvDesc.Texture2D.MostDetailedMip = 0;
//	ResourceBuilder::Instance.Get().device->GetDevice()->CreateShaderResourceView(this->tex2D.Get(), &srvDesc, this->srv.GetAddressOf());
//
//}

void ShadowPass::Init(Texture* dsTexture)
{
	this->dsTexture = dsTexture;
}

void ShadowPass::Bind()
{
	// 깊이 스텐실 뷰 지우기 및 뷰포트 설정
	float clearDepth = 1.0f;

	ResourceBuilder::Instance.Get().device->GetDeviceContext()->ClearDepthStencilView(
		this->dsTexture->GetDSV().Get(), D3D11_CLEAR_DEPTH, clearDepth, 0);

	D3D11_VIEWPORT viewport = { 0.0f, 0.0f, static_cast<float>(SM_SIZE), static_cast<float>(SM_SIZE), 0.0f, 1.0f };
	ResourceBuilder::Instance.Get().device->GetDeviceContext()->RSSetViewports(1, &viewport);

	// 깊이 스텐실 뷰 설정
	ResourceBuilder::Instance.Get().device->GetDeviceContext()->OMSetRenderTargets(0, nullptr , this->dsTexture->GetDSV().Get());
}
