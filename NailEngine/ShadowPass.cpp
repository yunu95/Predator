#include "ShadowPass.h"
#include "Struct.h"
#include "ResourceBuilder.h"
#include "ResourceManager.h"
#include "Device.h"
#include "Texture.h"
#include "VertexShader.h"
#include "PixelShader.h"

LazyObjects<ShadowPass> ShadowPass::Instance;

void ShadowPass::Init(Texture* dsTexture, VertexShader* vs, PixelShader* ps, VertexShader* skinnedVS)
{
    this->dsTexture = dsTexture;
    this->vs = vs;
    this->skinnedVS = skinnedVS;
    this->ps = ps;
}

void ShadowPass::Bind()
{
    this->vs->Bind();
    this->ps->Bind();


    // 깊이 스텐실 뷰 지우기 및 뷰포트 설정
    float clearDepth = 1.0f;

    ResourceBuilder::Instance.Get().device->GetDeviceContext()->ClearDepthStencilView(
        this->dsTexture->GetDSV().Get(), D3D11_CLEAR_DEPTH, clearDepth, 0);

    D3D11_VIEWPORT viewport = { 0.0f, 0.0f, static_cast<float>(SM_SIZE), static_cast<float>(SM_SIZE), 0.0f, 1.0f };
    ResourceBuilder::Instance.Get().device->GetDeviceContext()->RSSetViewports(1, &viewport);

    // 깊이 스텐실 뷰 설정
    //ResourceBuilder::Instance.Get().device->GetDeviceContext()->OMSetRenderTargets(0, nullptr , this->dsTexture->GetDSV().Get());
    ResourceBuilder::Instance.Get().device->GetDeviceContext()->OMSetRenderTargets(1, tempRTV->GetRTV().GetAddressOf(), this->dsTexture->GetDSV().Get());
}

void ShadowPass::SkinnedBind()
{
    this->skinnedVS->Bind();
    this->ps->Bind();
}
void ShadowPass::SetTempRTV(Texture* texture)
{
    tempRTV = texture;
}
