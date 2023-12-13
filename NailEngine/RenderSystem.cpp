#include "RenderSystem.h"

#include "NailEngine.h"
#include "Struct.h"
#include "ConstantBuffer.h"

#include "ResourceManager.h"
#include "Material.h"
#include "Mesh.h"
#include "Animation.h"

#include "NailCamera.h"
#include "RenderableManager.h"
#include "IRenderable.h"
#include "SKinnedMesh.h"

#include "ILight.h"
#include "LightManager.h"
#include "PointLight.h"

#include "NailCamera.h"
#include "RenderTargetGroup.h"

#include "Utils.h"

#include "ResourceBuilder.h"
#include "SwapChain.h"
#include "Device.h"

LazyObjects<RenderSystem> RenderSystem::Instance;


void RenderSystem::ClearRenderInfo()
{
	deferredVec.clear();
	forwardVec.clear();
	skinnedVec.clear();
}

void RenderSystem::SortObject()
{
	auto& staticRenderableSet = RenderableManager::Instance.Get().GetStaticRenderableSet();

	for (auto& e : staticRenderableSet)
	{
		auto mesh = e->GetMesh();
		for (int i = 0; i < mesh->GetMaterialCount(); ++i)
		{
			RenderInfo renderInfo;
			renderInfo.mesh = mesh;
			renderInfo.material = e->GetMaterial(i);
			renderInfo.materialIndex = i;
			renderInfo.wtm = e->GetWorldTM();

			if (e->GetMaterial(i)->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Deferred)
			{
				this->deferredVec.emplace_back(renderInfo);
			}
			else if (e->GetMaterial(i)->GetPixelShader()->GetShaderInfo().shaderType == yunuGI::ShaderType::Forward)
			{
				this->forwardVec.emplace_back(renderInfo);
			}
		}
	}

	// skinned
	auto& skinnedRenderableSet = RenderableManager::Instance.Get().GetSKinnedRenderableSet();

	for (auto& e : skinnedRenderableSet)
	{
		auto mesh = e->GetMesh();
		for (int i = 0; i < mesh->GetMaterialCount(); ++i)
		{
			SkinnedRenderInfo skinnedRenderInfo;

			RenderInfo renderInfo;
			renderInfo.mesh = mesh;
			renderInfo.material = e->GetMaterial(i);
			renderInfo.materialIndex = i;
			renderInfo.wtm = e->GetWorldTM();

			skinnedRenderInfo.renderInfo = std::move(renderInfo);

			skinnedRenderInfo.boneName = std::static_pointer_cast<SkinnedMesh>(e)->GetBone();

			this->skinnedVec.emplace_back(skinnedRenderInfo);
		}
	}
}

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
	ClearRenderInfo();
	SortObject();

	PushCameraData();
	PushLightData();

	// 스태틱 오브젝트 렌더
	RenderObject();

	// 스킨드 오브젝트 렌더
	RenderSkinned();
	
	// 라이트 렌더
	RenderLight();

	// Final 출력
	DrawFinal();

	RenderForward();

	// 디퍼드 정보 출력
	DrawDeferredInfo();

	// 디퍼드용 SRV UnBind
	std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(L"Deferred_DirectionalLight"))->UnBindGraphicsData();
	std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(L"Deferred_Final"))->UnBindGraphicsData();
}

void RenderSystem::RenderObject()
{
	auto& renderTargetGroup = NailEngine::Instance.Get().GetRenderTargetGroup();
	renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::G_BUFFER)]->OMSetRenderTarget();

	for (auto& e : this->deferredVec)
	{
		MatrixBuffer matrixBuffer;
		matrixBuffer.WTM = e.wtm;
		matrixBuffer.VTM = NailCamera::Instance.Get().GetVTM();
		matrixBuffer.PTM = NailCamera::Instance.Get().GetPTM();
		matrixBuffer.WVP = matrixBuffer.WTM * matrixBuffer.VTM * matrixBuffer.PTM;
		matrixBuffer.WorldInvTrans = matrixBuffer.WTM.Invert().Transpose();
		NailEngine::Instance.Get().GetConstantBuffer(0)->PushGraphicsData(&matrixBuffer, sizeof(MatrixBuffer), 0);

		auto mesh = std::static_pointer_cast<Mesh>(ResourceManager::Instance.Get().GetMesh(e.mesh->GetName()));

		std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(e.material->GetName()))->PushGraphicsData();
		mesh->Render(e.materialIndex);
	}
	
	//renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::G_BUFFER)]->UnBind();
}

void RenderSystem::RenderSkinned()
{
	for (auto& e : this->skinnedVec)
	{
		// 본TM 구해서 넘기기
		BoneUpdate(e);

		MatrixBuffer matrixBuffer;
		matrixBuffer.WTM = e.renderInfo.wtm;
		matrixBuffer.VTM = NailCamera::Instance.Get().GetVTM();
		matrixBuffer.PTM = NailCamera::Instance.Get().GetPTM();
		matrixBuffer.WVP = matrixBuffer.WTM * matrixBuffer.VTM * matrixBuffer.PTM;
		matrixBuffer.WorldInvTrans = matrixBuffer.WTM.Invert().Transpose();
		NailEngine::Instance.Get().GetConstantBuffer(0)->PushGraphicsData(&matrixBuffer, sizeof(MatrixBuffer), 0);

		auto mesh = std::static_pointer_cast<Mesh>(ResourceManager::Instance.Get().GetMesh(e.renderInfo.mesh->GetName()));

		auto shaderList = ResourceManager::Instance.Get().GetShaderList();
		for (auto& i : shaderList)
		{
			if (i->GetName() == L"SkinnedVS.cso")
			{
				std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(e.renderInfo.material->GetName()))->SetVertexShader(i);
			}
		}

		
		std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(e.renderInfo.material->GetName()))->PushGraphicsData();
		mesh->Render(e.renderInfo.materialIndex);
	}
}

void RenderSystem::RenderLight()
{
	auto& renderTargetGroup = NailEngine::Instance.Get().GetRenderTargetGroup();
	auto& lightSet = LightManager::Instance.Get().GetLightList();
	// 라이팅 렌더
	renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::LIGHTING)]->OMSetRenderTarget();
	for (auto& e : lightSet)
	{
		// Point Light의 경우 실제 Sphere Mesh를 렌더링 파이프라인에 넘긴다.
		// 이때 WVP이 필요하기에 상수버퍼에 밀어넣어야 한다.
		if (e->GetLightInfo().lightType == static_cast<unsigned int>(LightType::Point))
		{
			MatrixBuffer matrixBuffer;
			matrixBuffer.WTM = std::static_pointer_cast<PointLight>(e)->GetWorldTM();
			matrixBuffer.VTM = NailCamera::Instance.Get().GetVTM();
			matrixBuffer.PTM = NailCamera::Instance.Get().GetPTM();
			matrixBuffer.WVP = matrixBuffer.WTM * matrixBuffer.VTM * matrixBuffer.PTM;
			matrixBuffer.WorldInvTrans = std::static_pointer_cast<PointLight>(e)->GetWorldTM().Invert().Transpose();
			NailEngine::Instance.Get().GetConstantBuffer(0)->PushGraphicsData(&matrixBuffer, sizeof(MatrixBuffer), 0);
		}

		std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(e->GetMaterialName()))->SetInt(0, e->GetID());
		std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(e->GetMaterialName()))->PushGraphicsData();
		auto mesh = ResourceManager::Instance.Get().GetMesh(e->GetMeshName());
		mesh->Render();
	}

	//renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::LIGHTING)]->UnBind();
}

void RenderSystem::DrawFinal()
{
	// 렌더 타겟 뷰를 바인딩하고, 뎁스 스텐실 버퍼를 출력 병합기 단계에 바인딩한다.
	ResourceBuilder::Instance.Get().device->GetDeviceContext().Get()->OMSetRenderTargets(1,
		ResourceBuilder::Instance.Get().swapChain->GetRTV().GetAddressOf(),
		ResourceBuilder::Instance.Get().swapChain->GetDSV().Get());


	std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(L"Deferred_Final"))->PushGraphicsData();
	ResourceManager::Instance.Get().GetMesh(L"Rectangle")->Render();
}

void RenderSystem::RenderForward()
{
	for (auto& e : this->forwardVec)
	{
		MatrixBuffer matrixBuffer;
		matrixBuffer.WTM = e.wtm;
		matrixBuffer.VTM = NailCamera::Instance.Get().GetVTM();
		matrixBuffer.PTM = NailCamera::Instance.Get().GetPTM();
		matrixBuffer.WVP = matrixBuffer.WTM * matrixBuffer.VTM * matrixBuffer.PTM;
		matrixBuffer.WorldInvTrans = matrixBuffer.WTM.Invert().Transpose();
		NailEngine::Instance.Get().GetConstantBuffer(0)->PushGraphicsData(&matrixBuffer, sizeof(MatrixBuffer), 0);

		auto mesh = std::static_pointer_cast<Mesh>(ResourceManager::Instance.Get().GetMesh(e.mesh->GetName()));

		std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(e.material->GetName()))->PushGraphicsData();
		mesh->Render(e.materialIndex);
	}
}

void RenderSystem::DrawDeferredInfo()
{
	auto windowInfo = NailEngine::Instance.Get().GetWindowInfo();
	int width = windowInfo.width;
	int height = windowInfo.height;
	for (int i = 0; i < 6; i++)
	{
		DirectX::SimpleMath::Matrix matSclae = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(width / 10.f, height / 10.f, 1.f));
		DirectX::SimpleMath::Matrix matRotation = DirectX::SimpleMath::Matrix::CreateRotationX(0.f);
		matRotation *= DirectX::SimpleMath::Matrix::CreateRotationY(0.f);
		matRotation *= DirectX::SimpleMath::Matrix::CreateRotationZ(0.f);
		/// 
		DirectX::SimpleMath::Matrix matTranslation = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(
			((-width/2.f) + ((width/10.f)*(i+2)))
			,((height/2.f) - ((height/10.f)*2.f) + 22.f)
			,1.f)
		);
		/// 
		DirectX::SimpleMath::Matrix wtm = matSclae * matRotation * matTranslation;

		if (i == 0)
			std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(L"DeferredPosition"))->PushGraphicsData();
		else if (i == 1)
			std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(L"DeferredNormal"))->PushGraphicsData();
		else if (i == 2)
			std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(L"DeferredColor"))->PushGraphicsData();
		else if (i == 3)
			std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(L"DeferredDepth"))->PushGraphicsData();
		else if (i == 4)
			std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(L"DeferredDiffuseLight"))->PushGraphicsData();
		else if (i == 5)
			std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(L"DeferredSpecularLight"))->PushGraphicsData();
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

void RenderSystem::BoneUpdate(const SkinnedRenderInfo& skinnedRenderInfo)
{
	//auto& boneVec = ResourceManager::Instance.Get().GetFBXBoneData(std::string{ skinnedRenderInfo.boneName.begin(), skinnedRenderInfo.boneName.end() });
	//BoneMatrix* boneMatrixBuffer = new BoneMatrix;
	//for (int i = 0; i < boneVec.size(); ++i)
	//{
	//	if (i == 0)
	//	{
	//		boneMatrixBuffer->finalTM[i] = boneVec[i].offset * boneVec[i].localTM;
	//		temp[i] = boneVec[i].localTM;
	//	}
	//	else
	//	{
	//		//boneMatrixBuffer.finalTM[i] = boneVec[i].offset * boneMatrixBuffer.finalTM[boneVec[i].parentIndex-1] * boneVec[i].localTM;
	//		boneMatrixBuffer->finalTM[i] = boneVec[i].offset * boneVec[i].localTM * temp[boneVec[i].parentIndex - 1];
	//		temp[i] = boneVec[i].localTM * temp[boneVec[i].parentIndex - 1];

	//		assert(boneVec[i].index -1 == i);
	//		assert(boneVec[i].parentIndex - 1 < i);
	//	}
	//}

	//NailEngine::Instance.Get().GetConstantBuffer(4)->PushGraphicsData(boneMatrixBuffer, sizeof(BoneMatrix), 4);

	//delete boneMatrixBuffer;
}
