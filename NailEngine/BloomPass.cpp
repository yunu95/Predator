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
	DownSampling4x4_0();
	DownSampling6x6_0();

	Blur();

	UpSampling6x6_0();
	UpSampling4x4_0();

	//DownSampling6x6_1();

	//UpSampling6x6();
	//UpSampling6x6();

	//emissiveTexture->UnBind(static_cast<int>(yunuGI::Texture_Type::Temp0));
}

void BloomPass::DownSampling4x4_0()
{
	auto& renderTargetGroup = NailEngine::Instance.Get().GetRenderTargetGroup();
	renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::DOWN4X4_0)]->OMSetRenderTarget(true);

	D3D11_VIEWPORT viewport = { 0.0f, 0.0f, 
		static_cast<float>(NailEngine::Instance.Get().GetWindowInfo().width / 4),
		static_cast<float>(NailEngine::Instance.Get().GetWindowInfo().height /4),
		0.0f, 1.0f };
	ResourceBuilder::Instance.Get().device->GetDeviceContext()->RSSetViewports(1, &viewport);

	emissiveTexture->Bind(static_cast<int>(yunuGI::Texture_Type::Temp0));

	vs->Bind();
	copyPS->Bind();

	quadMesh->Render();
}

void BloomPass::DownSampling6x6_0()
{
	auto& renderTargetGroup = NailEngine::Instance.Get().GetRenderTargetGroup();
	renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::DOWN6X6_0)]->OMSetRenderTarget(true);

	D3D11_VIEWPORT viewport = { 0.0f, 0.0f, 
		static_cast<float>((NailEngine::Instance.Get().GetWindowInfo().width / 4)/6),
		static_cast<float>((NailEngine::Instance.Get().GetWindowInfo().height / 4)/6),
		0.0f, 1.0f };
	ResourceBuilder::Instance.Get().device->GetDeviceContext()->RSSetViewports(1, &viewport);

	renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::DOWN4X4_0)]->GetRTTexture(DOWN4X4_0)->Bind(static_cast<int>(yunuGI::Texture_Type::Temp0));

	vs->Bind();
	copyPS->Bind();

	quadMesh->Render();
}

void BloomPass::DownSampling6x6_1()
{
	auto& renderTargetGroup = NailEngine::Instance.Get().GetRenderTargetGroup();
	renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::DOWN6X6_1)]->OMSetRenderTarget(true);

	D3D11_VIEWPORT viewport = { 0.0f, 0.0f,
		static_cast<float>(((NailEngine::Instance.Get().GetWindowInfo().width / 4) / 6)/6),
		static_cast<float>(((NailEngine::Instance.Get().GetWindowInfo().height / 4) / 6)/6),
		0.0f, 1.0f };
	ResourceBuilder::Instance.Get().device->GetDeviceContext()->RSSetViewports(1, &viewport);

	renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::DOWN6X6_0)]->GetRTTexture(DOWN6X6_0)->Bind(static_cast<int>(yunuGI::Texture_Type::Temp0));

	vs->Bind();
	copyPS->Bind();

	quadMesh->Render();
}

void BloomPass::UpSampling6x6_0()
{
	auto& renderTargetGroup = NailEngine::Instance.Get().GetRenderTargetGroup();
	renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::UP6x6_0)]->OMSetRenderTarget(true);

	D3D11_VIEWPORT viewport = { 0.0f, 0.0f,
		static_cast<float>(((NailEngine::Instance.Get().GetWindowInfo().width / 4))),
		static_cast<float>(((NailEngine::Instance.Get().GetWindowInfo().height / 4))),
		0.0f, 1.0f };
	ResourceBuilder::Instance.Get().device->GetDeviceContext()->RSSetViewports(1, &viewport);

	renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::BLUR)]->GetRTTexture(BLUR)->Bind(static_cast<int>(yunuGI::Texture_Type::Temp0));

	vs->Bind();
	copyPS->Bind();

	quadMesh->Render();
}

void BloomPass::UpSampling4x4_0()
{
	auto& renderTargetGroup = NailEngine::Instance.Get().GetRenderTargetGroup();
	renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::UP4x4_0)]->OMSetRenderTarget(true);

	D3D11_VIEWPORT viewport = { 0.0f, 0.0f,
		static_cast<float>(((NailEngine::Instance.Get().GetWindowInfo().width))),
		static_cast<float>(((NailEngine::Instance.Get().GetWindowInfo().height))),
		0.0f, 1.0f };
	ResourceBuilder::Instance.Get().device->GetDeviceContext()->RSSetViewports(1, &viewport);

	renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::UP6x6_0)]->GetRTTexture(UP6x6_0)->Bind(static_cast<int>(yunuGI::Texture_Type::Temp0));

	vs->Bind();
	copyPS->Bind();

	quadMesh->Render();
}

void BloomPass::Blur()
{
	auto& renderTargetGroup = NailEngine::Instance.Get().GetRenderTargetGroup();
	renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::BLUR)]->OMSetRenderTarget(true);

	D3D11_VIEWPORT viewport = { 0.0f, 0.0f,
		static_cast<float>(((NailEngine::Instance.Get().GetWindowInfo().width / 4) / 6)),
		static_cast<float>(((NailEngine::Instance.Get().GetWindowInfo().height / 4) / 6)),
		0.0f, 1.0f };
	ResourceBuilder::Instance.Get().device->GetDeviceContext()->RSSetViewports(1, &viewport);

	renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::DOWN6X6_0)]->GetRTTexture(DOWN6X6_0)->Bind(static_cast<int>(yunuGI::Texture_Type::Temp0));

	vs->Bind();
	std::static_pointer_cast<PixelShader>(ResourceManager::Instance.Get().GetDeferredShader(L"BlurPS.cso"))->Bind();

	quadMesh->Render();
}
