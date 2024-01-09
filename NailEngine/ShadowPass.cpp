#include "ShadowPass.h"

#include "ResourceBuilder.h"
#include "Device.h"

LazyObjects<ShadowPass> ShadowPass::Instance;

void ShadowPass::Init()
{
	// Depth Stencil Buffer (D3D11_BIND_DEPTH_STENCIL)
	D3D11_TEXTURE2D_DESC depthStencilDescDS;
	ZeroMemory(&depthStencilDescDS, sizeof(depthStencilDescDS));
	depthStencilDescDS.Width = SM_SIZE;
	depthStencilDescDS.Height = SM_SIZE;
	depthStencilDescDS.MipLevels = 1;
	depthStencilDescDS.ArraySize = 1;
	depthStencilDescDS.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // 예시로 D24_UNORM_S8_UINT를 사용
	depthStencilDescDS.SampleDesc.Count = 1;
	depthStencilDescDS.SampleDesc.Quality = 0;
	depthStencilDescDS.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDescDS.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	ResourceBuilder::Instance.Get().device->GetDevice()->CreateTexture2D(&depthStencilDescDS, nullptr, this->dsvTex2D.GetAddressOf());

	ResourceBuilder::Instance.Get().device->GetDevice()->CreateDepthStencilView(this->dsvTex2D.Get(), nullptr, this->dsv.GetAddressOf());

	// Depth Stencil Buffer (D3D11_BIND_SHADER_RESOURCE)
	D3D11_TEXTURE2D_DESC depthStencilDescSRV;
	ZeroMemory(&depthStencilDescSRV, sizeof(depthStencilDescSRV));
	depthStencilDescSRV.Width = SM_SIZE;
	depthStencilDescSRV.Height = SM_SIZE;
	depthStencilDescSRV.MipLevels = 1;
	depthStencilDescSRV.ArraySize = 1;
	depthStencilDescSRV.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS; // 예시로 R24_UNORM_X8_TYPELESS를 사용
	depthStencilDescSRV.SampleDesc.Count = 1;
	depthStencilDescSRV.SampleDesc.Quality = 0;
	depthStencilDescSRV.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDescSRV.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	ResourceBuilder::Instance.Get().device->GetDevice()->CreateTexture2D(&depthStencilDescSRV, nullptr, this->srvTex2D.GetAddressOf());

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
	shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS; // 예시로 형식 지정
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;

	ResourceBuilder::Instance.Get().device->GetDevice()->CreateShaderResourceView(this->srvTex2D.Get(), &shaderResourceViewDesc, this->srv.GetAddressOf());
}

void ShadowPass::Bind()
{
	// 깊이 스텐실 뷰 지우기 및 뷰포트 설정
	float clearDepth = 1.0f;
	ResourceBuilder::Instance.Get().device->GetDeviceContext()->ClearDepthStencilView(this->dsv.Get(), D3D11_CLEAR_DEPTH, clearDepth, 0);

	//D3D11_VIEWPORT viewport = { 0.0f, 0.0f, static_cast<float>(SM_SIZE), static_cast<float>(SM_SIZE), 0.0f, 1.0f };
	//ResourceBuilder::Instance.Get().device->GetDeviceContext()->RSSetViewports(1, &viewport);

	// 깊이 스텐실 뷰 설정
	ResourceBuilder::Instance.Get().device->GetDeviceContext()->OMSetRenderTargets(0, nullptr, this->dsv.Get());
}
