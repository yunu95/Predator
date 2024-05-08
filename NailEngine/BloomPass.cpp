#include "BloomPass.h"

#include "ResourceManager.h"
#include "NailEngine.h"
#include "RenderTargetGroup.h"

#include "Texture.h"
#include "Mesh.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "ResourceBuilder.h"
#include "Device.h"

LazyObjects<BloomPass> BloomPass::Instance;

void BloomPass::Init()
{
	vs = std::static_pointer_cast<VertexShader>(ResourceManager::Instance.Get().GetDeferredShader(L"Deferred_FinalVS.cso"));
	copyPS = std::static_pointer_cast<PixelShader>(ResourceManager::Instance.Get().GetDeferredShader(L"CopyPS.cso"));

	auto& renderTargetGroup = NailEngine::Instance.Get().GetRenderTargetGroup();
	emissiveTexture = renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::G_BUFFER)]->GetRTTexture(static_cast<int>(EMISSIVE));

	quadMesh = ResourceManager::Instance.Get().GetMesh(L"Rectangle");
}

void BloomPass::Bloom()
{
	DownSampling2x2_0();
	DownSampling2x2_1();
	DownSampling2x2_2();
	Blur();
	UpSampling2x2_0();
	UpSampling2x2_1();
	UpSampling2x2_2();
}

void BloomPass::DownSampling2x2_0()
{
	auto& renderTargetGroup = NailEngine::Instance.Get().GetRenderTargetGroup();
	renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::DOWN2X2_0)]->OMSetRenderTarget(true);

	D3D11_VIEWPORT viewport = { 0.0f, 0.0f,
		static_cast<float>(NailEngine::Instance.Get().GetWindowInfo().width / 2),
		static_cast<float>(NailEngine::Instance.Get().GetWindowInfo().height / 2),
		0.0f, 1.0f };

	ResourceBuilder::Instance.Get().device->GetDeviceContext()->RSSetViewports(1, &viewport);

	emissiveTexture->Bind(static_cast<int>(yunuGI::Texture_Type::Temp0));

	vs->Bind();
	copyPS->Bind();

	quadMesh->Render();
}

void BloomPass::DownSampling2x2_1()
{
	auto& renderTargetGroup = NailEngine::Instance.Get().GetRenderTargetGroup();
	renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::DOWN2X2_1)]->OMSetRenderTarget(true);

	D3D11_VIEWPORT viewport = { 0.0f, 0.0f,
		static_cast<float>(NailEngine::Instance.Get().GetWindowInfo().width / 2) / 2,
		static_cast<float>(NailEngine::Instance.Get().GetWindowInfo().height / 2) / 2,
		0.0f, 1.0f };

	ResourceBuilder::Instance.Get().device->GetDeviceContext()->RSSetViewports(1, &viewport);

	renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::DOWN2X2_0)]->GetRTTexture(DOWN2X2_0)->Bind(static_cast<int>(yunuGI::Texture_Type::Temp0));

	vs->Bind();
	copyPS->Bind();

	quadMesh->Render();
}

void BloomPass::DownSampling2x2_2()
{
	auto& renderTargetGroup = NailEngine::Instance.Get().GetRenderTargetGroup();
	renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::DOWN2X2_2)]->OMSetRenderTarget(true);

	D3D11_VIEWPORT viewport = { 0.0f, 0.0f,
		static_cast<float>(((NailEngine::Instance.Get().GetWindowInfo().width / 2) / 2) / 2),
		static_cast<float>(((NailEngine::Instance.Get().GetWindowInfo().height / 2) / 2) / 2),
		0.0f, 1.0f };

	ResourceBuilder::Instance.Get().device->GetDeviceContext()->RSSetViewports(1, &viewport);

	renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::DOWN2X2_1)]->GetRTTexture(DOWN2X2_1)->Bind(static_cast<int>(yunuGI::Texture_Type::Temp0));

	vs->Bind();
	copyPS->Bind();

	quadMesh->Render();
}

void BloomPass::UpSampling2x2_0()
{
	auto& renderTargetGroup = NailEngine::Instance.Get().GetRenderTargetGroup();
	renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::UP2X2_0)]->OMSetRenderTarget(true);

	D3D11_VIEWPORT viewport = { 0.0f, 0.0f,
		static_cast<float>(((NailEngine::Instance.Get().GetWindowInfo().width / 2) / 2)),
		static_cast<float>(((NailEngine::Instance.Get().GetWindowInfo().height / 2) / 2)),
		0.0f, 1.0f };

	ResourceBuilder::Instance.Get().device->GetDeviceContext()->RSSetViewports(1, &viewport);

	renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::BLUR)]->GetRTTexture(BLUR)->Bind(static_cast<int>(yunuGI::Texture_Type::Temp0));

	vs->Bind();
	copyPS->Bind();

	quadMesh->Render();
}

void BloomPass::UpSampling2x2_1()
{
	auto& renderTargetGroup = NailEngine::Instance.Get().GetRenderTargetGroup();
	renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::UP2X2_1)]->OMSetRenderTarget(true);

	D3D11_VIEWPORT viewport = { 0.0f, 0.0f,
		static_cast<float>(((NailEngine::Instance.Get().GetWindowInfo().width / 2))),
		static_cast<float>(((NailEngine::Instance.Get().GetWindowInfo().height / 2))),
		0.0f, 1.0f };

	ResourceBuilder::Instance.Get().device->GetDeviceContext()->RSSetViewports(1, &viewport);

	renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::UP2X2_0)]->GetRTTexture(UP2X2_0)->Bind(static_cast<int>(yunuGI::Texture_Type::Temp0));

	vs->Bind();
	copyPS->Bind();

	quadMesh->Render();
}

void BloomPass::UpSampling2x2_2()
{
	auto& renderTargetGroup = NailEngine::Instance.Get().GetRenderTargetGroup();
	renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::UP2X2_2)]->OMSetRenderTarget(true);

	D3D11_VIEWPORT viewport = { 0.0f, 0.0f,
		static_cast<float>(((NailEngine::Instance.Get().GetWindowInfo().width))),
		static_cast<float>(((NailEngine::Instance.Get().GetWindowInfo().height))),
		0.0f, 1.0f };

	ResourceBuilder::Instance.Get().device->GetDeviceContext()->RSSetViewports(1, &viewport);

	renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::UP2X2_1)]->GetRTTexture(UP2X2_1)->Bind(static_cast<int>(yunuGI::Texture_Type::Temp0));

	vs->Bind();
	copyPS->Bind();

	quadMesh->Render();
}

void BloomPass::Blur()
{
	auto& renderTargetGroup = NailEngine::Instance.Get().GetRenderTargetGroup();
	renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::BLUR)]->OMSetRenderTarget(true);

	D3D11_VIEWPORT viewport = { 0.0f, 0.0f,
		static_cast<float>((((NailEngine::Instance.Get().GetWindowInfo().width / 2) / 2) / 2)),
		static_cast<float>((((NailEngine::Instance.Get().GetWindowInfo().height / 2) / 2) / 2)),
		0.0f, 1.0f };

	ResourceBuilder::Instance.Get().device->GetDeviceContext()->RSSetViewports(1, &viewport);

	renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::DOWN2X2_2)]->GetRTTexture(DOWN2X2_2)->Bind(static_cast<int>(yunuGI::Texture_Type::Temp0));

	vs->Bind();
	std::static_pointer_cast<PixelShader>(ResourceManager::Instance.Get().GetDeferredShader(L"BlurPS.cso"))->Bind();

	quadMesh->Render();
}
