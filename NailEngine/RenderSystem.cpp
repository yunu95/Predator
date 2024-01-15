#include "RenderSystem.h"

#include "NailEngine.h"

#include "ConstantBuffer.h"

#include "ResourceManager.h"
#include "Mesh.h"
#include "Animation.h"
#include "AnimationGroup.h"

#include "NailCamera.h"
#include "RenderableManager.h"
#include "IRenderable.h"
#include "SKinnedMesh.h"

#include "ILight.h"
#include "LightManager.h"
#include "PointLight.h"
#include "DirectionalLight.h"

#include "NailCamera.h"
#include "RenderTargetGroup.h"

#include "Utils.h"

#include "ResourceBuilder.h"
#include "SwapChain.h"
#include "Device.h"

#include "NailAnimatorManager.h"
#include "NailAnimator.h"

#include "InstancingManager.h"

#include "UIImage.h"
#include "Texture.h"


#include "ShadowPass.h"

#include <iostream>
#include <fstream>

LazyObjects<RenderSystem> RenderSystem::Instance;

void RenderSystem::Init()
{

	spriteBatch = std::make_unique<DirectX::SpriteBatch>(ResourceBuilder::Instance.Get().device->GetDeviceContext().Get());
	commonStates = std::make_unique<DirectX::CommonStates>(ResourceBuilder::Instance.Get().device->GetDevice().Get());

	auto& shaderList = ResourceManager::Instance.Get().GetShaderList();
	for (auto& i : shaderList)
	{
		if (i->GetName() == L"TestPS.cso")
		{
			this->ps = i;
		}
		if (i->GetName() == L"TestVS.cso")
		{
			this->vs = i;
		}
	}
}

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
		if (e->IsActive() == false)
		{
			continue;
		}

		auto mesh = e->GetMesh();
		for (int i = 0; i < mesh->GetMaterialCount(); ++i)
		{
			RenderInfo renderInfo;
			renderInfo.mesh = mesh;
			renderInfo.material = e->GetMaterial(i);
			renderInfo.shadowMaterial = Material(*e->GetMaterial(i));
			renderInfo.shadowMaterial.SetPixelShader(this->ps);
			renderInfo.shadowMaterial.SetVertexShader(this->vs);
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
		if (e->IsActive() == false)
		{
			continue;
		}

		auto mesh = e->GetMesh();
		for (int i = 0; i < mesh->GetMaterialCount(); ++i)
		{
			SkinnedRenderInfo skinnedRenderInfo;

			RenderInfo renderInfo;
			renderInfo.mesh = mesh;
			renderInfo.material = e->GetMaterial(i);
			renderInfo.materialIndex = i;
			renderInfo.wtm = e->GetWorldTM();
			//renderInfo.objecID = e->GetID();
			skinnedRenderInfo.animator = NailAnimatorManager::Instance.Get().GetAnimator(
				std::static_pointer_cast<SkinnedMesh>(e)->GetAnimatorIndex());

			skinnedRenderInfo.renderInfo = std::move(renderInfo);

			skinnedRenderInfo.modelName = std::static_pointer_cast<SkinnedMesh>(e)->GetBone();

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
		else if (e->GetLightInfo().lightType == static_cast<unsigned int>(LightType::Spot))
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

	// 그림자 맵 생성
	RenderShadow();

	// 라이트 렌더
	RenderLight();

	// Final 출력
	DrawFinal();

	RenderForward();

	RenderUI();

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

	MatrixBuffer matrixBuffer;
	//matrixBuffer.WTM = e.wtm;
	matrixBuffer.VTM = NailCamera::Instance.Get().GetVTM();
	matrixBuffer.PTM = NailCamera::Instance.Get().GetPTM();
	matrixBuffer.WVP = matrixBuffer.WTM * matrixBuffer.VTM * matrixBuffer.PTM;
	matrixBuffer.WorldInvTrans = matrixBuffer.WTM.Invert().Transpose();
	//matrixBuffer.objectID = DirectX::SimpleMath::Vector4{};
	NailEngine::Instance.Get().GetConstantBuffer(0)->PushGraphicsData(&matrixBuffer, sizeof(MatrixBuffer), 0);

	InstancingManager::Instance.Get().RegisterMeshAndMaterial(this->deferredVec);

	//for (auto& e : this->deferredVec)
	//{
	//	

	//	/*auto mesh = std::static_pointer_cast<Mesh>(ResourceManager::Instance.Get().GetMesh(e.mesh->GetName()));

	//	std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(e.material->GetName()))->PushGraphicsData();
	//	mesh->Render(e.materialIndex);*/
	//	
	//}

	//renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::G_BUFFER)]->UnBind();
}

void RenderSystem::RenderSkinned()
{
	MatrixBuffer matrixBuffer;
	//matrixBuffer.WTM = e.wtm;
	matrixBuffer.VTM = NailCamera::Instance.Get().GetVTM();
	matrixBuffer.PTM = NailCamera::Instance.Get().GetPTM();
	matrixBuffer.WVP = matrixBuffer.WTM * matrixBuffer.VTM * matrixBuffer.PTM;
	matrixBuffer.WorldInvTrans = matrixBuffer.WTM.Invert().Transpose();
	//matrixBuffer.objectID = DirectX::SimpleMath::Vector4{};
	NailEngine::Instance.Get().GetConstantBuffer(0)->PushGraphicsData(&matrixBuffer, sizeof(MatrixBuffer), 0);

	InstancingManager::Instance.Get().RegisterSkinnedMeshAndMaterial(this->skinnedVec);

	//for (auto& e : this->skinnedVec)
	//{
	//	// 본TM 구해서 넘기기
	//	//BoneUpdate(e);

	//	auto animator = NailAnimatorManager::Instance.Get().GetAnimator(e.animatorIndex);
	//	auto modelName = animator->GetModel();
	//	ResourceManager::Instance.Get().GetAnimationGroup(modelName)->Bind();

	//	//KeyframeDesc keyFrameDesc;
	//	//keyFrameDesc.animIndex = animator->GetTransitionDesc().curr.animIndex;
	//	//keyFrameDesc.currFrame = animator->GetCurrentFrame();
	//	//keyFrameDesc.nextFrame = animator->GetCurrentFrame()+1;
	//	//keyFrameDesc.ratio = animator->GetFrameRatio();
	//	auto& transitionDesc = animator->GetTransitionDesc();
	//	NailEngine::Instance.Get().GetConstantBuffer(5)->PushGraphicsData(&transitionDesc, sizeof(InstanceTransitionDesc), 5);

	//	MatrixBuffer matrixBuffer;
	//	matrixBuffer.WTM = e.renderInfo.wtm;
	//	matrixBuffer.VTM = NailCamera::Instance.Get().GetVTM();
	//	matrixBuffer.PTM = NailCamera::Instance.Get().GetPTM();
	//	matrixBuffer.WVP = matrixBuffer.WTM * matrixBuffer.VTM * matrixBuffer.PTM;
	//	matrixBuffer.WorldInvTrans = matrixBuffer.WTM.Invert().Transpose();
	//	//matrixBuffer.objectID = DirectX::SimpleMath::Vector4{e.renderInfo.objecID,0,0,0};
	//	NailEngine::Instance.Get().GetConstantBuffer(0)->PushGraphicsData(&matrixBuffer, sizeof(MatrixBuffer), 0);

	//	auto mesh = std::static_pointer_cast<Mesh>(ResourceManager::Instance.Get().GetMesh(e.renderInfo.mesh->GetName()));

	//	auto animationGroup = ResourceManager::Instance.Get().GetAnimationGroup(e.modelName);
	//	animationGroup->Bind();

	//	std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(e.renderInfo.material->GetName()))->PushGraphicsData();
	//	mesh->Render(e.renderInfo.materialIndex,nullptr);
	//}
}

void RenderSystem::RenderShadow()
{
	auto& lightSet = LightManager::Instance.Get().GetLightList();
	ShadowPass::Instance.Get().Bind();
	//auto& renderTargetGroup = NailEngine::Instance.Get().GetRenderTargetGroup();
	//renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::SHADOW)]->OMSetRenderTarget();

	//MatrixBuffer matrixBuffer;
	//matrixBuffer.VTM = NailCamera::Instance.Get().GetVTM();
	//matrixBuffer.PTM = NailCamera::Instance.Get().GetPTM();
	//matrixBuffer.WVP = matrixBuffer.WTM * matrixBuffer.VTM * matrixBuffer.PTM;
	//matrixBuffer.WorldInvTrans = matrixBuffer.WTM.Invert().Transpose();
	//NailEngine::Instance.Get().GetConstantBuffer(0)->PushGraphicsData(&matrixBuffer, sizeof(MatrixBuffer), 0);
	for (auto& e : lightSet)
	{
		if (e->GetLightInfo().lightType == static_cast<unsigned int>(LightType::Directional))
		{
			MatrixBuffer matrixBuffer;
			matrixBuffer.VTM = std::static_pointer_cast<DirectionalLight>(e)->GetWorldTM().Invert();
			matrixBuffer.PTM = DirectX::XMMatrixOrthographicLH(100 * 1.f, 100 * 1.f, 0.0000001f, 500.f);
			//matrixBuffer.PTM = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PI/4.f, 1920/ 1080, 0.1f, 1000.f);
			NailEngine::Instance.Get().GetConstantBuffer(0)->PushGraphicsData(&matrixBuffer, sizeof(MatrixBuffer), 0);
		}
	}
	InstancingManager::Instance.Get().RegisterMeshAndShadowMaterial(this->deferredVec);
}

void RenderSystem::RenderLight()
{
	auto& renderTargetGroup = NailEngine::Instance.Get().GetRenderTargetGroup();
	auto& lightSet = LightManager::Instance.Get().GetLightList();
	// 라이팅 렌더
	renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::LIGHTING)]->OMSetRenderTarget();

	MatrixBuffer matrixBuffer;
	matrixBuffer.VTM = NailCamera::Instance.Get().GetVTM();
	matrixBuffer.PTM = NailCamera::Instance.Get().GetPTM();
	matrixBuffer.WVP = matrixBuffer.WTM * matrixBuffer.VTM * matrixBuffer.PTM;
	

	for (auto& e : lightSet)
	{
		// Point Light의 경우 실제 Sphere Mesh를 렌더링 파이프라인에 넘긴다.
		// 이때 WVP이 필요하기에 상수버퍼에 밀어넣어야 한다.
		if (e->GetLightInfo().lightType == static_cast<unsigned int>(LightType::Directional))
		{
			matrixBuffer.VTMInv = matrixBuffer.VTM.Invert();
			matrixBuffer.lightVP = std::static_pointer_cast<DirectionalLight>(e)->GetWorldTM().Invert() * DirectX::XMMatrixOrthographicLH(100 * 1.f, 100 * 1.f, 0.0000001f, 500.f);
			//matrixBuffer.lightVP = std::static_pointer_cast<DirectionalLight>(e)->GetWorldTM().Invert() * DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PI / 4.f, 1920 / 1080, 0.1f, 1000.f);
		}
		else if (e->GetLightInfo().lightType == static_cast<unsigned int>(LightType::Point))
		{
			matrixBuffer.WTM = std::static_pointer_cast<PointLight>(e)->GetWorldTM();
			matrixBuffer.WorldInvTrans = std::static_pointer_cast<PointLight>(e)->GetWorldTM().Invert().Transpose();
		}
		NailEngine::Instance.Get().GetConstantBuffer(0)->PushGraphicsData(&matrixBuffer, sizeof(MatrixBuffer), 0);
		std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(e->GetMaterialName()))->SetInt(0, e->GetID());
		std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(e->GetMaterialName()))->PushGraphicsData();
		auto mesh = ResourceManager::Instance.Get().GetMesh(e->GetMeshName());
		mesh->Render();

		//renderTargetGroup[static_cast<int>(RENDER_TARGET_TYPE::LIGHTING)]->UnBind();
	}
}

void RenderSystem::DrawFinal()
{
	// 렌더 타겟 뷰를 바인딩하고, 뎁스 스텐실 버퍼를 출력 병합기 단계에 바인딩한다.
	ResourceBuilder::Instance.Get().device->GetDeviceContext()->OMSetRenderTargets(1,
		ResourceBuilder::Instance.Get().swapChain->GetRTV().GetAddressOf(),
		ResourceBuilder::Instance.Get().swapChain->GetDSV().Get());

	auto& lightSet = LightManager::Instance.Get().GetLightList();
	for (auto& e : lightSet)
	{
		if (e->GetLightInfo().lightType == static_cast<unsigned int>(LightType::Directional))
		{
			MatrixBuffer matrixBuffer;
			matrixBuffer.VTM = std::static_pointer_cast<DirectionalLight>(e)->GetWorldTM().Invert();
			matrixBuffer.PTM = DirectX::XMMatrixOrthographicLH(500 * 1.f, 500 * 1.f, 0.1f, 150.f);
			NailEngine::Instance.Get().GetConstantBuffer(0)->PushGraphicsData(&matrixBuffer, sizeof(MatrixBuffer), 0);
		}
	}

	std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(L"Deferred_Final"))->PushGraphicsData();
	ResourceManager::Instance.Get().GetMesh(L"Rectangle")->Render();
}

void RenderSystem::RenderUI()
{
	auto& uiSet = RenderableManager::Instance.Get().GetUIImageSet();
	this->spriteBatch->Begin(DirectX::SpriteSortMode_Deferred, this->commonStates->NonPremultiplied());
	for (auto& i : uiSet)
	{
		auto uiImage = std::static_pointer_cast<UIImage>(i);

		if (uiImage->IsActive() == false)
		{
			continue;
		}

		auto texture = ((Texture*)(std::static_pointer_cast<UIImage>(i)->GetTexture()));
		this->spriteBatch->Draw(texture->GetSRV().Get(), uiImage->pos);
	}
	this->spriteBatch->End();
}

void RenderSystem::RenderForward()
{
	//MatrixBuffer matrixBuffer;
	////matrixBuffer.WTM = e.wtm;
	//matrixBuffer.VTM = NailCamera::Instance.Get().GetVTM();
	//matrixBuffer.PTM = NailCamera::Instance.Get().GetPTM();
	//matrixBuffer.WVP = matrixBuffer.WTM * matrixBuffer.VTM * matrixBuffer.PTM;
	//matrixBuffer.WorldInvTrans = matrixBuffer.WTM.Invert().Transpose();
	////matrixBuffer.objectID = DirectX::SimpleMath::Vector4{};
	//NailEngine::Instance.Get().GetConstantBuffer(0)->PushGraphicsData(&matrixBuffer, sizeof(MatrixBuffer), 0);

	//for (auto& e : this->forwardVec)
	//{
	//	MatrixBuffer matrixBuffer;
	//	matrixBuffer.WTM = e.wtm;
	//	matrixBuffer.VTM = NailCamera::Instance.Get().GetVTM();
	//	matrixBuffer.PTM = NailCamera::Instance.Get().GetPTM();
	//	matrixBuffer.WVP = matrixBuffer.WTM * matrixBuffer.VTM * matrixBuffer.PTM;
	//	matrixBuffer.WorldInvTrans = matrixBuffer.WTM.Invert().Transpose();
	//	NailEngine::Instance.Get().GetConstantBuffer(0)->PushGraphicsData(&matrixBuffer, sizeof(MatrixBuffer), 0);
	//
	//	auto mesh = std::static_pointer_cast<Mesh>(ResourceManager::Instance.Get().GetMesh(e.mesh->GetName()));
	//
	//	std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(e.material->GetName()))->PushGraphicsData();
	//	mesh->Render(e.materialIndex, nullptr);
	//}

	MatrixBuffer matrixBuffer;
	//matrixBuffer.WTM = e.wtm;
	matrixBuffer.VTM = NailCamera::Instance.Get().GetVTM();
	matrixBuffer.PTM = NailCamera::Instance.Get().GetPTM();
	matrixBuffer.WVP = matrixBuffer.WTM * matrixBuffer.VTM * matrixBuffer.PTM;
	matrixBuffer.WorldInvTrans = matrixBuffer.WTM.Invert().Transpose();
	//matrixBuffer.objectID = DirectX::SimpleMath::Vector4{};
	NailEngine::Instance.Get().GetConstantBuffer(0)->PushGraphicsData(&matrixBuffer, sizeof(MatrixBuffer), 0);
	InstancingManager::Instance.Get().RegisterMeshAndMaterial(this->forwardVec);
}

void RenderSystem::DrawDeferredInfo()
{
	auto windowInfo = NailEngine::Instance.Get().GetWindowInfo();
	int width = windowInfo.width;
	int height = windowInfo.height;
	for (int i = 0; i < 7; i++)
	{
		DirectX::SimpleMath::Matrix matSclae = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(width / 10.f, height / 10.f, 1.f));
		DirectX::SimpleMath::Matrix matRotation = DirectX::SimpleMath::Matrix::CreateRotationX(0.f);
		matRotation *= DirectX::SimpleMath::Matrix::CreateRotationY(0.f);
		matRotation *= DirectX::SimpleMath::Matrix::CreateRotationZ(0.f);
		/// 
		DirectX::SimpleMath::Matrix matTranslation = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(
			((-width / 2.f) + ((width * 0.05) + ((width / 10.f) * i)))
			, ((height / 2.f) - ((height * 0.05)))
			, 1.f));
		/// 


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
		else if (i == 6)
		{
			DirectX::SimpleMath::Matrix matTranslation = DirectX::SimpleMath::Matrix::CreateTranslation(DirectX::SimpleMath::Vector3(
				((-SM_SIZE / 2.f) + ((SM_SIZE * 0.05) + ((SM_SIZE / 10.f) * i)))
				, ((SM_SIZE / 2.f) - ((SM_SIZE * 0.05)))
				, 1.f)
			);
			matSclae = DirectX::SimpleMath::Matrix::CreateScale(DirectX::SimpleMath::Vector3(SM_SIZE / 10.f, SM_SIZE / 10.f, 1.f));
			std::static_pointer_cast<Material>(ResourceManager::Instance.Get().GetMaterial(L"DeferredShadow"))->PushGraphicsData();
		}
		DirectX::SimpleMath::Matrix wtm = matSclae * matRotation * matTranslation;
		MatrixBuffer matrixBuffer;
		matrixBuffer.WVP = wtm * DirectX::SimpleMath::Matrix::Identity * NailCamera::Instance.Get().GetVTMOrtho();

		NailEngine::Instance.Get().GetConstantBuffer(0)->PushGraphicsData(&matrixBuffer, sizeof(MatrixBuffer), 0);
		ResourceManager::Instance.Get().GetMesh(L"Rectangle")->Render();
	}
}

//void RenderSystem::BoneUpdate(const SkinnedRenderInfo& skinnedRenderInfo)
//{
//	/*auto& boneMap = ResourceManager::Instance.Get().GetFBXBoneData(std::string{ skinnedRenderInfo.modelName.begin(), skinnedRenderInfo.modelName.end() });
//
//	
//
//	auto fbxNode = ResourceManager::Instance.Get().GetFBXNode(skinnedRenderInfo.modelName);
//
//	ReadBone(fbxNode, DirectX::SimpleMath::Matrix::Identity, std::string{ skinnedRenderInfo.modelName.begin(), skinnedRenderInfo.modelName.end() }, animator);
//
//	NailEngine::Instance.Get().GetConstantBuffer(4)->PushGraphicsData(&this->finalTM, sizeof(BoneMatrix), 4);*/
//}

//void RenderSystem::ReadBone(FBXNode* fbxNode, DirectX::SimpleMath::Matrix parentMatrix, const std::string& fbxName, std::shared_ptr<NailAnimator> animator)
//{
//	auto& boneInfoMap = ResourceManager::Instance.Get().GetFBXBoneData(fbxName);
//
//	Animation* animation = static_cast<Animation*>(animator->GetCurrentAnimation());
//	int currentFrame = animator->GetCurrentFrame();
//	int nextFrame = currentFrame + 1;
//	float frameRatio = animator->GetFrameRatio();
//	AnimationClip& animationClip = animation->GetAnimationClip();
//
//	DirectX::SimpleMath::Matrix srt = fbxNode->transformMatrix;
//
//	auto iter = boneInfoMap.find(fbxNode->nodeName);
//	if (iter != boneInfoMap.end())
//	{
//		auto animationPos = DirectX::SimpleMath::Vector3::Lerp(animationClip.keyFrameInfoVec[iter->second.index][currentFrame].pos,
//			animationClip.keyFrameInfoVec[iter->second.index][nextFrame].pos, frameRatio);
//
//		auto animationScale = DirectX::SimpleMath::Vector3::Lerp(animationClip.keyFrameInfoVec[iter->second.index][currentFrame].scale,
//			animationClip.keyFrameInfoVec[iter->second.index][nextFrame].scale, frameRatio);
//
//		auto animationRot = DirectX::SimpleMath::Quaternion::Slerp(animationClip.keyFrameInfoVec[iter->second.index][currentFrame].rot,
//			animationClip.keyFrameInfoVec[iter->second.index][nextFrame].rot, frameRatio);
//
//		auto translateMat = DirectX::SimpleMath::Matrix::CreateTranslation(animationPos);
//		auto rotationMat = DirectX::XMMatrixRotationQuaternion(animationRot);
//		auto scaleMat = DirectX::SimpleMath::Matrix::CreateScale(animationScale);
//
//		srt = scaleMat * rotationMat * translateMat;
//
//		this->finalTM.finalTM[iter->second.index] = iter->second.offset * srt * parentMatrix;
//	}
//
//	for (int i = 0; i < fbxNode->child.size(); ++i)
//	{
//		ReadBone(fbxNode->child[i], srt * parentMatrix, fbxName, animator);
//	}
//}
