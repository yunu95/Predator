#include "RenderSystem.h"

#include <DirectXMath.h>
#include "SimpleMath.h"
using namespace DirectX::PackedVector;

#include "NailEngine.h"
#include "Struct.h"
#include "ConstantBuffer.h"

#include "ResourceManager.h"
#include "Material.h"
#include "Mesh.h"

#include "NailCamera.h"
#include "RenderableManager.h"
#include "IRenderable.h"

#include "ILight.h"
#include "LightManager.h"

#include "NailCamera.h"
#include "RenderTargetGroup.h"

#include "Utils.h"

#include "ResourceBuilder.h"
#include "SwapChain.h"
#include "Device.h"

LazyObjects<RenderSystem> RenderSystem::Instance;

void RenderSystem::PushLightData()
{
	auto& lightSet = LightManager::Instance.Get().GetLightList();
	LightParams params;
	params.lightCount = LightManager::Instance.Get().GetLightCount();

	unsigned int i = 0;

	for (auto& e : lightSet)
	{
		params.lights[i] = e->GetLightInfo();

		if (e->GetLightInfo().lightType == static_cast<unsigned int>(LightType::Directional))
		{
			params.lights[i].direction = e->GetLightInfo().direction;
		}
		else if (e->GetLightInfo().lightType == static_cast<unsigned int>(LightType::Point))
		{
			params.lights[i].range = e->GetLightInfo().range;
		}
		else if(e->GetLightInfo().lightType == static_cast<unsigned int>(LightType::Spot))
		{
			params.lights[i].range = e->GetLightInfo().range;
			params.lights[i].angle = e->GetLightInfo().angle;
			params.lights[i].direction = e->GetLightInfo().direction;
		}
		
		i++;
	}

	NailEngine::Instance.Get().GetConstantBuffer(2)->PushGraphicsData(&params, sizeof(LightParams), 2);
}

void RenderSystem::PushCameraData()
{
	CameraBuffer buffer;
	DirectX::SimpleMath::Vector3 pos;
	DirectX::SimpleMath::Vector3 scale;
	DirectX::SimpleMath::Quaternion quat;
	NailCamera::Instance.Get().GetWTM().Decompose(scale, quat, pos);
	buffer.position = pos;
	NailEngine::Instance.Get().GetConstantBuffer(3)->PushGraphicsData(&buffer, sizeof(CameraBuffer), 3);
}

void RenderSystem::Render()
{
	PushCameraData();
	PushLightData();

	// 오브젝트 렌더
	RenderObject();
	
	// 라이트 렌더
	RenderLight();

	// Final 출력
	DrawFinal();

	// 디퍼드 정보 출력
	DrawDeferredInfo();

	// 디퍼드용 SRV UnBind
	ResourceManager::Instance.Get().GetMaterial(L"Deferred_DirectionalLight")->UnBindGraphicsData();
	ResourceManager::Instance.Get().GetMaterial(L"Deferred_Final")->UnBindGraphicsData();
}

void RenderSystem::RenderObject()
{
	
	auto& renderableSet = RenderableManager::Instance.Get().GetRenderableSet();
	auto& renderTargetGroup = NailEngine::Instance.Get().GetRenderTargetGroup();
	renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::G_BUFFER)]->OMSetRenderTarget();

	for (auto& e : renderableSet)
	{
		MatrixBuffer matrixBuffer;
		matrixBuffer.WTM = e->GetWorldTM();
		matrixBuffer.VTM = NailCamera::Instance.Get().GetVTM();
		matrixBuffer.PTM = NailCamera::Instance.Get().GetPTM();
		matrixBuffer.WVP = matrixBuffer.WTM * matrixBuffer.VTM * matrixBuffer.PTM;
		matrixBuffer.WorldInvTrans = e->GetWorldTM().Invert().Transpose();
		NailEngine::Instance.Get().GetConstantBuffer(0)->PushGraphicsData(&matrixBuffer, sizeof(MatrixBuffer), 0);

		auto& mesh = ResourceManager::Instance.Get().GetMesh(e->GetMeshName());
		for (int i = 0; i < mesh->GetMaterialCount(); ++i)
		{
			ResourceManager::Instance.Get().GetMaterial(e->GetMaterial(i)->GetMaterialName())->PushGraphicsData();
			mesh->Render(i);
		}
	}
	
	//renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::G_BUFFER)]->UnBind();
}

void RenderSystem::RenderLight()
{
	auto& renderTargetGroup = NailEngine::Instance.Get().GetRenderTargetGroup();
	auto& lightSet = LightManager::Instance.Get().GetLightList();
	// 라이팅 렌더
	renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::LIGHTING)]->OMSetRenderTarget();
	for (auto& e : lightSet)
	{
		ResourceManager::Instance.Get().GetMaterial(e->GetMaterialName())->PushGraphicsData();
		auto& mesh = ResourceManager::Instance.Get().GetMesh(e->GetMeshName());
		for (int i = 0; i < mesh->GetMaterialCount(); ++i)
		{
			mesh->Render(i);
		}
	}
	//renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::LIGHTING)]->UnBind();
}

void RenderSystem::DrawFinal()
{
	// 렌더 타겟 뷰를 바인딩하고, 뎁스 스텐실 버퍼를 출력 병합기 단계에 바인딩한다.
	ResourceBuilder::Instance.Get().device->GetDeviceContext().Get()->OMSetRenderTargets(1,
		ResourceBuilder::Instance.Get().swapChain->GetRTV().GetAddressOf(),
		ResourceBuilder::Instance.Get().swapChain->GetDSV().Get());

	ResourceManager::Instance.Get().GetMaterial(L"Deferred_Final")->PushGraphicsData();
	ResourceManager::Instance.Get().GetMesh(L"Rectangle")->Render();
}

void RenderSystem::DrawDeferredInfo()
{
	for (int i = 0; i < 6; i++)
	{
		DirectX::SimpleMath::Matrix matSclae = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(1280 / 10.f, 800 / 10.f, 1.f));
		DirectX::SimpleMath::Matrix matRotation = DirectX::SimpleMath::Matrix::CreateRotationX(0.f);
		matRotation *= DirectX::SimpleMath::Matrix::CreateRotationY(0.f);
		matRotation *= DirectX::SimpleMath::Matrix::CreateRotationZ(0.f);
		DirectX::SimpleMath::Matrix matTranslation = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(-576.f + (i * 128.f), 360.f, 1.f));
		DirectX::SimpleMath::Matrix wtm = matSclae * matRotation * matTranslation;

		if (i == 0)
			ResourceManager::Instance.Get().GetMaterial(L"DeferredPosition")->PushGraphicsData();
		else if (i == 1)
			ResourceManager::Instance.Get().GetMaterial(L"DeferredNormal")->PushGraphicsData();
		else if (i == 2)
			ResourceManager::Instance.Get().GetMaterial(L"DeferredColor")->PushGraphicsData();
		else if (i == 3)
			ResourceManager::Instance.Get().GetMaterial(L"DeferredDepth")->PushGraphicsData();
		else if (i == 4)
			ResourceManager::Instance.Get().GetMaterial(L"DeferredDiffuseLight")->PushGraphicsData();
		else if (i == 5)
			ResourceManager::Instance.Get().GetMaterial(L"DeferredSpecularLight")->PushGraphicsData();
		//else if (i == 6)
		//{
		//	matSclae = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(1280 / 10.f, 800 / 10.f, 1.f));
		//	matTranslation = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(-865.f + (i * 200), 424.f, 500.f));
		//	wtm = matSclae * matRotation * matTranslation;
		//	ResourceManager::Instance.Get().GetMaterial(L"ShadowTarget")->PushGraphicsData();
		//}

		MatrixBuffer matrixBuffer;
		matrixBuffer.WVP = wtm * DirectX::SimpleMath::Matrix::Identity * NailCamera::Instance.Get().GetVTMOrtho();

		NailEngine::Instance.Get().GetConstantBuffer(0)->PushGraphicsData(&matrixBuffer, sizeof(MatrixBuffer), 0);
		ResourceManager::Instance.Get().GetMesh(L"Rectangle")->Render();
	}
}
